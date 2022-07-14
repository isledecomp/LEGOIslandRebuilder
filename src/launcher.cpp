#include "launcher.h"

#include <COMMDLG.H>
#include <SHLWAPI.H>

#include "../res/resource.h"

HANDLE Launcher::Launch(HWND parent)
{
  // Find the installation
  TCHAR filename[MAX_PATH];
  if (!FindInstallation(parent, filename)) {
    // Don't show error message because presumably the user cancelled out of looking for ISLE
    return NULL;
  }

  // Set compatibility flags
  HKEY hKey;
  LPCTSTR str = TEXT("~ HIGHDPIAWARE 16BITCOLOR DWM8And16BitMitigation");
  if (RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
    // Often we seem to get an 8.3 path which is not valid here, so resolve the full path now
    TCHAR full_path[MAX_PATH];
    GetLongPathName(filename, full_path, MAX_PATH);
    MessageBoxA(0,full_path,0,0);

    LONG ret = RegSetValueEx(hKey, full_path, 0, REG_SZ, (const BYTE *) str, strlen(str)+1);
    if (ret != ERROR_SUCCESS) {
      char buf[100];
      sprintf(buf, "Failed to set compatibility flags: 0x%x. This may cause issues on newer versions of Windows.", ret);
      MessageBox(parent, buf, 0, 0);
    }

    RegCloseKey(hKey);
  }


  // Extract REBLD.DLL which contains our patches
  TCHAR libraryFile[MAX_PATH];
  if (!ExtractLibrary(libraryFile, MAX_PATH)) {
    MessageBox(parent, "Failed to extract to temp", NULL, 0);
    return NULL;
  }

  // Get ISLE directory only
  TCHAR srcDir[MAX_PATH];
  _tcscpy(srcDir, filename);
  PathRemoveFileSpec(srcDir);

  // Start launching our copy
  PROCESS_INFORMATION pi;
  if (!TryCreateProcess(parent, filename, srcDir, TRUE, &pi)) {
    return NULL;
  }

  UINT_PTR inject_addr = 0x0040843A;
  UINT_PTR old_addr;
  ReadProcessMemory(pi.hProcess, (LPVOID) inject_addr, &old_addr, sizeof(UINT_PTR), NULL);
  old_addr += inject_addr + 4;

  // Set up x86 code template
  char *inject_func = "\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE9\xBA\xBA\xBA\xBA";
  size_t inject_func_sz = 15;

  // Write DLL address
  UINT_PTR addr = (UINT_PTR) 0x0040E5D6;
  UINT_PTR string_addr = addr + inject_func_sz;
  int dllPathLength = strlen(libraryFile) + 1;
  WriteProcessMemory(pi.hProcess, (LPVOID) string_addr, libraryFile, dllPathLength, NULL);

  // Fill in addresses in code template
  memcpy(inject_func + 1, &string_addr, sizeof(string_addr));
  UINT_PTR loadLibrary = (UINT_PTR) GetProcAddress(GetModuleHandle("KERNEL32.DLL"), "LoadLibraryA");
  loadLibrary -= addr + 10;
  memcpy(inject_func + 6, &loadLibrary, sizeof(loadLibrary));
  old_addr -= addr + 15;
  memcpy(inject_func + 11, &old_addr, sizeof(old_addr));

  // Write code
  WriteProcessMemory(pi.hProcess, (LPVOID) addr, inject_func, inject_func_sz, NULL);

  // Replace function call
  addr -= (UINT_PTR)(inject_addr) + 4;
  WriteProcessMemory(pi.hProcess, (LPVOID) inject_addr, &addr, sizeof(addr), NULL);

  // Resume process thread
  ResumeThread(pi.hThread);

  return pi.hProcess;
}

BOOL Launcher::FindInstallation(HWND parent, LPTSTR str)
{
  // Search for LEGO Island disk path
  DWORD value_sz;

  BOOL success = TRUE;

  // Start with empty string
  str[0] = 0;

  // On install, LEGO Island records its installation directory in the registry
  HKEY hKey;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Mindscape\\LEGO Island", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
    LONG ret = RegQueryValueEx(hKey, "diskpath", NULL, NULL, NULL, &value_sz);
    if (ret == ERROR_SUCCESS) {
      // Get value from registry
      RegQueryValueEx(hKey, "diskpath", NULL, NULL, (BYTE*)str, &value_sz);

      // Append ISLE.EXE to diskpath
      _tcscat(str, "\\ISLE.EXE");
    }

    RegCloseKey(hKey);
  }

  // Validate diskpath, either we couldn't find the registry entry or it was incorrect somehow
  while (!_tcslen(str) || !PathFileExists(str)) {
    // Ask user where LEGO Island is installed
    OPENFILENAME fn;
    ZeroMemory(&fn, sizeof(fn));
    fn.hwndOwner = parent;
    fn.lStructSize = sizeof(fn);
    fn.lpstrFile = str;
    fn.lpstrFile[0] = '\0';
    fn.nMaxFile = MAX_PATH;
    fn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    fn.lpstrTitle = "Where is LEGO Island installed?";
    fn.lpstrFilter = "ISLE.EXE\0ISLE.EXE\0";

    if (!GetOpenFileName(&fn)) {
      // If they cancelled the dialog, break out of the loop
      success = FALSE;
      break;
    }
  }

  return success;
}

BOOL Launcher::ExtractLibrary(LPTSTR str, SIZE_T len)
{
  // Find temporary location to store DLL
  if (!GetTempPath(len, str)) {
    return FALSE;
  }

  _tcscat(str, "REBLD.DLL");

  // Open file
  HANDLE file = CreateFile(str, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (!file) {
    return FALSE;
  }

  // Extract DLL which should have been compiled in as a resource
  HRSRC res = FindResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(WORKER_DLL), RT_RCDATA);
  if (!res) {
    return FALSE;
  }

  HGLOBAL res_resource = LoadResource(NULL, res);
  if (!res_resource) {
    return FALSE;
  }

  LPVOID res_data = LockResource(res_resource);
  DWORD res_sz = SizeofResource(NULL, res);

  DWORD writtenBytes;
  BOOL success = WriteFile(file, res_data, res_sz, &writtenBytes, NULL);

  UnlockResource(res_resource);

  FreeResource(res_resource);

  CloseHandle(file);

  return success;
}

BOOL Launcher::CopyIsleToTemp(LPCTSTR src, LPTSTR dst)
{
  BOOL success = FALSE;

  TCHAR tempDir[MAX_PATH];
  if (GetTempPath(MAX_PATH, tempDir)) {
    _tcscpy(dst, tempDir);
    _tcscat(dst, "ISLE.EXE");
    if (CopyFile(src, dst, FALSE)) {
      // Force our copy to load our DLL
      success = TRUE;
    }
  }

  return success;
}

BOOL Launcher::PatchIsle(LPCTSTR filename)
{
  BOOL success = false;

  if (HANDLE file = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0)) {
    // Force ISLE to load our DLL. This system will work on all platforms including Windows 95.
    if (ReplacePatternInFile(file, "timeGetTime\0WINMM", "rbldGetTime\0REBLD", 17)) {
      success = true;
    }

    CloseHandle(file);
  }

  return success;
}

BOOL Launcher::ReplacePatternInFile(HANDLE file, const char *pattern, const char *replace, LONG sz)
{
  BOOL success = FALSE;

  SetFilePointer(file, 0, 0, FILE_BEGIN);

  char *data = new char[sz];

  // Find pattern in the file
  DWORD nbBytesRead;
  do {
    ReadFile(file, data, sz, &nbBytesRead, NULL);
    SetFilePointer(file, -sz + 1, 0, FILE_CURRENT);
  } while ((LONG)nbBytesRead == sz && memcmp(data, pattern, sz));

  if ((LONG)nbBytesRead == sz) {
    // Must have found pattern
    SetFilePointer(file, -1, 0, FILE_CURRENT);

    // Overwrite with replace
    DWORD nbBytesWritten; // We don't use this value, but Windows 95 will fail without it
    if (WriteFile(file, replace, sz, &nbBytesWritten, NULL)) {
      success = TRUE;
    } else {
      char buf[200];
      sprintf(buf, "Failed to write to file with error: %lx", GetLastError());
      MessageBoxA(0, buf, 0, 0);
    }
  }

  delete [] data;

  return success;
}

BOOL Launcher::TryCreateProcess(HWND parent, LPSTR filename, LPCSTR working_dir, BOOL suspended, PROCESS_INFORMATION *pi)
{
  STARTUPINFO si;

  ZeroMemory(pi, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&si, sizeof(si));

  if (!CreateProcess(NULL, filename, NULL, NULL, FALSE, suspended ? CREATE_SUSPENDED : 0, NULL, working_dir, &si, pi)) {
    TCHAR err[2048];
    _stprintf(err, "Failed to create process with error 0x%lx", GetLastError());
    MessageBox(parent, err, NULL, 0);
    return FALSE;
  }

  return TRUE;
}

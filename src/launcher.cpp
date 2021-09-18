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

  // If we found it, make a copy to temp
  TCHAR copiedFile[MAX_PATH];
  TCHAR libraryFile[MAX_PATH];
  if (!CopyIsleToTemp(filename, copiedFile)) {
    MessageBox(parent, _T("Failed to copy to temp"), NULL, 0);
    return NULL;
  }

  // Extract REBLD.DLL which contains our patches
  if (!ExtractLibrary(libraryFile, MAX_PATH)) {
    MessageBox(parent, _T("Failed to extract to temp"), NULL, 0);
    return NULL;
  }

  // Patch our copied ISLE to import our DLL
  if (!PatchIsle(copiedFile)) {
    MessageBox(parent, _T("Failed to patch import"), NULL, 0);
    return NULL;
  }

  // Get ISLE directory only
  TCHAR srcDir[MAX_PATH];
  _tcscpy(srcDir, filename);
  PathRemoveFileSpec(srcDir);

  // Start launching our copy
  PROCESS_INFORMATION pi;
  STARTUPINFO si;

  ZeroMemory(&pi, sizeof(pi));
  ZeroMemory(&si, sizeof(si));

  if (!CreateProcess(NULL, copiedFile, NULL, NULL, FALSE, 0, NULL, srcDir, &si, &pi)) {
    TCHAR err[2048];
    _stprintf(err, _T("Failed to create process with error 0x%lx"), GetLastError());
    MessageBox(parent, err, NULL, 0);
    return NULL;
  }

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
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Mindscape\\LEGO Island"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
    LONG ret = RegQueryValueEx(hKey, _T("diskpath"), NULL, NULL, NULL, &value_sz);
    if (ret == ERROR_SUCCESS) {
      // Get value from registry
      RegQueryValueEx(hKey, _T("diskpath"), NULL, NULL, (BYTE*)str, &value_sz);

      // Append ISLE.EXE to diskpath
      _tcscat(str, _T("\\ISLE.EXE"));
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
    fn.lpstrTitle = _T("Where is LEGO Island installed?");
    fn.lpstrFilter = _T("ISLE.EXE\0ISLE.EXE\0");

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

  _tcscat(str, _T("REBLD.DLL"));

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
    _tcscat(dst, _T("ISLE.EXE"));
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

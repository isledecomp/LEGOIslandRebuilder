#include "launcher.h"

#include <COMMDLG.H>
#include <SHLWAPI.H>

HANDLE Launcher::Launch(HWND parent)
{
  HANDLE ret = NULL;

  // Find the installation
  LPTSTR filename = FindInstallation(parent);

  if (filename) {
    // If we found it, launch now
    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));

    if (CreateProcess(NULL, filename, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
      ret = pi.hProcess;

      if (!Patch(ret) && MessageBox(parent, _T("One or more patches failed. Would you like to continue?"), NULL, MB_YESNO) == IDNO) {
        // Something went wrong, so we'll terminate this process now
        TerminateProcess(ret, 1);
        ret = NULL;
      } else {
        // Otherwise resume
        ResumeThread(pi.hThread);
      }
    } else {
      TCHAR err[2048];
      _stprintf(err, _T("Failed to create process with error 0x%lx"), GetLastError());
      MessageBox(parent, err, NULL, 0);
    }
  }

  delete [] filename;

  return ret;
}

LPTSTR Launcher::FindInstallation(HWND parent)
{
  // Search for LEGO Island disk path
  DWORD value_sz;

  TCHAR *isle_diskpath = NULL;

  // On install, LEGO Island records its installation directory in the registry
  HKEY hKey;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Mindscape\\LEGO Island"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
    LONG ret = RegQueryValueEx(hKey, _T("diskpath"), NULL, NULL, NULL, &value_sz);
    if (ret == ERROR_SUCCESS) {
      // Get value from registry
      TCHAR *reg_val = new TCHAR[value_sz];
      RegQueryValueEx(hKey, _T("diskpath"), NULL, NULL, (BYTE*)reg_val, &value_sz);

      // Append ISLE.EXE to diskpath
      isle_diskpath = new TCHAR[MAX_PATH];
      wsprintf(isle_diskpath, _T("%s\\ISLE.EXE"), reg_val);

      // Delete reg val
      delete [] reg_val;
    }

    RegCloseKey(hKey);
  }

  // Validate diskpath, either we couldn't find the registry entry or it was incorrect somehow
  while (!isle_diskpath || !PathFileExists(isle_diskpath)) {
    if (!isle_diskpath) {
      // Allocate diskpath if it hasn't been allocated yet
      isle_diskpath = new TCHAR[MAX_PATH];
    }

    // Ask user where LEGO Island is installed
    OPENFILENAME fn;
    ZeroMemory(&fn, sizeof(fn));
    fn.hwndOwner = parent;
    fn.lStructSize = sizeof(fn);
    fn.lpstrFile = isle_diskpath;
    fn.lpstrFile[0] = '\0';
    fn.nMaxFile = MAX_PATH;
    fn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    fn.lpstrTitle = _T("Where is LEGO Island installed?");
    fn.lpstrFilter = _T("ISLE.EXE\0ISLE.EXE\0");

    if (!GetOpenFileName(&fn)) {
      // If they cancelled the dialog, break out of the loop
      delete [] isle_diskpath;
      isle_diskpath = NULL;
      break;
    }
  }

  return isle_diskpath;
}

BOOL Launcher::Patch(HANDLE process)
{
  UINT32 width = 320;
  UINT32 height = 240;

  if (!WriteProcessMemory(process, (LPVOID)0x00410048, &width, sizeof(width), NULL)) {
    return FALSE;
  }

  if (!WriteProcessMemory(process, (LPVOID)0x0041004C, &height, sizeof(height), NULL)) {
    return FALSE;
  }

  return TRUE;
}

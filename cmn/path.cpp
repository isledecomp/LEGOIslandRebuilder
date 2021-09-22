#include "path.h"

#include <SHLOBJ.H>
#include <SHLWAPI.H>
#include <STRING>
#include <TCHAR.H>

LPCTSTR appName = TEXT("Rebuilder");

BOOL DirectoryExists(LPCTSTR szPath)
{
  return PathFileExists(szPath) && PathIsDirectory(szPath);
}

BOOL RecursivelyCreateDirectory(LPCTSTR directory)
{
  if (DirectoryExists(directory)) {
    // Directory already exists, do nothing
    return TRUE;
  } else {
    // Determine directory of this directory
    std::basic_string<TCHAR> copy = directory;
    PathRemoveFileSpec(&copy[0]);

    // Create if necessary
    if (RecursivelyCreateDirectory(copy.c_str())) {
      return CreateDirectory(directory, NULL);
    } else {
      return FALSE;
    }
  }
}

#ifdef UNICODE
typedef BOOL (WINAPI *SHGetSpecialFolderPathSignature)(HWND hwndOwner, LPWSTR lpszPath, int nFolder, BOOL fCreate);
#else
typedef BOOL (WINAPI *SHGetSpecialFolderPathSignature)(HWND hwndOwner, LPSTR lpszPath, int nFolder, BOOL fCreate);
#endif
BOOL GetAppDataPath(LPTSTR s)
{
  OSVERSIONINFO info;
  ZeroMemory(&info, sizeof(info));
  info.dwOSVersionInfoSize = sizeof(info);
  GetVersionEx(&info);

  // Dynamically link to SHGetSpecialFolderPath because not all versions of Windows have it
#ifdef UNICODE
  LPCSTR functionName = "SHGetSpecialFolderPathW";
#else
  LPCSTR functionName = "SHGetSpecialFolderPathA";
#endif

  SHGetSpecialFolderPathSignature GetSpecialFolderPath = (SHGetSpecialFolderPathSignature)GetProcAddress(GetModuleHandle(_T("SHELL32.DLL")), functionName);
  BOOL haveDir = FALSE;
  BOOL usedShell = FALSE;
  if (GetSpecialFolderPath) {
    haveDir = GetSpecialFolderPath(NULL, s, CSIDL_APPDATA, TRUE);
    usedShell = TRUE;
  } else {
    // Assume we're on Windows 95 which has no application data folder, we bodge it to write to
    // "C:\Windows\Application Data" which is roughly where 98/Me would do it
    GetWindowsDirectory(s, MAX_PATH);
    _tcscat(s, _T("\\Application Data"));
    haveDir = TRUE;
  }

  //MessageBox(0, s, usedShell ? _T("Using API") : _T("Is this Windows 95?"), 0);
  return haveDir;
}

BOOL GetConfigFilename(LPTSTR s)
{
  if (GetAppDataPath(s)) {
    _tcscat(s, _T("\\LEGOIslandRebuilder"));
    if (RecursivelyCreateDirectory(s)) {
      _tcscat(s, _T("\\settings.ini"));
      return TRUE;
    }
  }

  return FALSE;
}

BOOL GetSafeLEGOIslandSavePath(LPTSTR s)
{
  if (GetAppDataPath(s)) {
    _tcscat(s, _T("\\LEGO Island"));
    if (RecursivelyCreateDirectory(s)) {
      return TRUE;
    }
  }

  return FALSE;
}

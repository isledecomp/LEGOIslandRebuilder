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
typedef BOOL (WINAPI *SHGetSpecialFolderPath_t)(HWND hwndOwner, LPWSTR lpszPath, int nFolder, BOOL fCreate);
#else
typedef BOOL (WINAPI *SHGetSpecialFolderPath_t)(HWND hwndOwner, LPSTR lpszPath, int nFolder, BOOL fCreate);
#endif
BOOL GetAppDataPath(LPTSTR s)
{
  // Dynamically link to SHGetSpecialFolderPath because not all versions of Windows have it
#ifdef UNICODE
  LPCSTR functionName = "SHGetSpecialFolderPathW";
#else
  LPCSTR functionName = "SHGetSpecialFolderPathA";
#endif

  SHGetSpecialFolderPath_t getSpecialFolderPath = (SHGetSpecialFolderPath_t)GetProcAddress(LoadLibrary(_T("SHELL32.DLL")), functionName);
  BOOL haveDir = FALSE;
  BOOL usedShell = FALSE;
  if (getSpecialFolderPath) {
    haveDir = getSpecialFolderPath(NULL, s, CSIDL_APPDATA, TRUE);
    usedShell = TRUE;
  } else {
    // Assume we're on Windows 95 which has no application data folder, we bodge it to write to
    // "C:\Windows\Application Data" which is roughly where 98/Me would do it
    GetWindowsDirectory(s, MAX_PATH);
    _tcscat(s, _T("\\Application Data"));
    haveDir = TRUE;
  }

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

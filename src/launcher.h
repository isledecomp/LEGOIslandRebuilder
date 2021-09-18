#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <AFXWIN.H>

class Launcher
{
public:
  Launcher();

  static HANDLE Launch(HWND parent);

private:
  static BOOL FindInstallation(HWND parent, LPTSTR isle_diskpath);

  static BOOL ExtractLibrary(LPTSTR str, SIZE_T len);

  static BOOL CopyIsleToTemp(LPCTSTR src, LPTSTR dst);

  static BOOL PatchIsle(LPCTSTR filename);

  static BOOL ReplacePatternInFile(HANDLE file, const char *pattern, const char *replace, LONG sz);

};

#endif // LAUNCHER_H

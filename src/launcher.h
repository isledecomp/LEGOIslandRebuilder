#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <AFXWIN.H>

class Launcher
{
public:
  Launcher();

  static HANDLE Launch(HWND parent);

private:
  static LPTSTR FindInstallation(HWND parent);

  static BOOL Patch(HANDLE process);

};

#endif // LAUNCHER_H

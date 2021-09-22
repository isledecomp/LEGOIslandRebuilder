#ifndef PATH_H
#define PATH_H

#include <WINDOWS.H>

BOOL GetSafeLEGOIslandSavePath(LPTSTR s);

BOOL GetConfigFilename(LPTSTR s);

extern LPCTSTR appName;

#endif // PATH_H

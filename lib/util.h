#ifndef UTIL_H
#define UTIL_H

#include <WINDOWS.H>

BOOL WriteMemory(LPVOID destination, LPVOID source, size_t length, LPVOID oldData = NULL);

BOOL OverwriteCall(LPVOID destination, LPVOID localCall);

SIZE_T SearchReplacePattern(LPVOID imageBase, LPCVOID search, LPCVOID replace, SIZE_T count, BOOL only_once = FALSE);

LPVOID OverwriteImport(LPVOID imageBase, LPCSTR overrideFunction, LPVOID override);

LPVOID OverwriteVirtualTable(LPVOID object, SIZE_T methodIndex, LPVOID overrideFunction);

#endif // UTIL_H

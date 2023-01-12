#ifndef UTIL_H
#define UTIL_H

#include <WINDOWS.H>

BOOL WriteMemory(LPVOID destination, LPVOID source, size_t length, LPVOID oldData = NULL);

LPVOID OverwriteCall(LPVOID destination, LPVOID localCall);

int OverwriteAllCalls(LPVOID imageBase, LPCVOID from, LPCVOID to);

LPVOID SearchPattern(LPVOID imageBase, LPCVOID search, SIZE_T count);

SIZE_T SearchReplacePattern(LPVOID imageBase, LPCVOID search, LPCVOID replace, SIZE_T count, BOOL only_once = FALSE);

LPVOID OverwriteImport(LPVOID imageBase, LPCSTR overrideFunction, LPVOID override);

template <typename T>
T PointerToMemberFunction(LPVOID ptr)
{
  union U {
    LPVOID in;
    T out;
  };

  U u;
  u.in = ptr;
  return u.out;
}

LPVOID OverwriteVirtualTable(LPVOID object, SIZE_T methodIndex, LPVOID overrideFunction);

#endif // UTIL_H

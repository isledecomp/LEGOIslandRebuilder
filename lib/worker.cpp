#include "worker.h"

#include <VECTOR>

#include "hooks.h"
#include "util.h"

DWORD WINAPI Patch()
{
  MessageBoxA(0,"Connect debugger now",0,0);

  // Hook import address table
  LPVOID exeBase = GetModuleHandle(TEXT("ISLE.EXE"));
  LPVOID dllBase = GetModuleHandle(TEXT("LEGO1.DLL"));

  // Redirect various imports
  OverwriteImport(exeBase, "CreateWindowExA", (LPVOID)InterceptCreateWindowExA);
  OverwriteImport(exeBase, "FindWindowA", (LPVOID)InterceptFindWindowA);
  OverwriteImport(dllBase, "OutputDebugStringA", (LPVOID)InterceptOutputDebugStringA);
  ddCreateOriginal = (ddCreateFunction)OverwriteImport(dllBase, "DirectDrawCreate", (LPVOID)InterceptDirectDrawCreate);

  // Stay active when defocused
  SearchReplacePattern(exeBase, "\x89\x58\x70", "\x90\x90\x90", 3);
  SearchReplacePattern(dllBase, "\xC7\x44\x24\x24\xE0\x00\x00\x00", "\xC7\x44\x24\x24\xE0\x80\x00\x00", 8);
  SearchReplacePattern(dllBase, "\xC7\x44\x24\x24\xB0\x00\x00\x00", "\xC7\x44\x24\x24\xB0\x80\x00\x00", 8);
  SearchReplacePattern(dllBase, "\xC7\x45\xCC\x11\x00\x00\x00", "\xC7\x45\xCC\x11\x80\x00\x00", 7);
  SearchReplacePattern(dllBase, "\xC7\x45\xCC\xE0\x00\x00\x00", "\xC7\x45\xCC\xE0\x80\x00\x00", 7);

  // DDRAW GetSurfaceDesc Override
  //OverwriteCall((LPVOID) ((UINT_PTR)dllBase+0xBA7D5), (LPVOID)InterceptSurfaceGetDesc);

  // Window size hack
  /*SearchReplacePattern(exeBase,
                       "\x80\x02\x00\x00\xE0\x01\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x80\x02\x00\x00\xE0\x01\x00\x00",
                       "\x40\x01\x00\x00\xE0\x01\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x80\x02\x00\x00\xE0\x01\x00\x00", 24);*/

  /*char debug[5];
  debug[0] = 0xE8;
  *(UINT_PTR*)(&debug[1]) = (UINT_PTR)InterceptOutputDebugStringA;
  if (SIZE_T replacements = SearchReplacePattern(legoBase, "\xE8\x34\x08\x00\x00", debug, 5)) {
    printf("Hooked some other debug function %lu times", replacements);
  }*/

  return 0;
}

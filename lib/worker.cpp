#include "worker.h"

#include <TCHAR.H>
#include <VECTOR>

#include "config.h"
#include "hooks.h"
#include "util.h"

DWORD WINAPI Patch()
{
  if (!config.Load()) {
    MessageBoxA(0, "Failed to find Rebuilder configuration. No patches will be active.", 0, 0);
    return 1;
  }

  // Hook import address table
  LPVOID exeBase = GetModuleHandle(TEXT("ISLE.EXE"));
  LPVOID dllBase = GetModuleHandle(TEXT("LEGO1.DLL"));

  // Redirect various imports
  OverwriteImport(exeBase, "CreateWindowExA", (LPVOID)InterceptCreateWindowExA);
  OverwriteImport(dllBase, "OutputDebugStringA", (LPVOID)InterceptOutputDebugStringA);
  OverwriteImport(exeBase, "RegQueryValueExA", (LPVOID)InterceptRegQueryValueExA);
  ddCreateOriginal = (ddCreateFunction)OverwriteImport(dllBase, "DirectDrawCreate", (LPVOID)InterceptDirectDrawCreate);

  // Stay active when defocused
  if (config.GetInt(_T("StayActiveWhenDefocused"))) {
    // Patch jump if window isn't active
    SearchReplacePattern(exeBase, "\x89\x58\x70", "\x90\x90\x90", 3);

    // Patch DirectSound flags so that sound doesn't mute when inactive
    SearchReplacePattern(dllBase, "\xC7\x44\x24\x24\xE0\x00\x00\x00", "\xC7\x44\x24\x24\xE0\x80\x00\x00", 8);
    SearchReplacePattern(dllBase, "\xC7\x44\x24\x24\xB0\x00\x00\x00", "\xC7\x44\x24\x24\xB0\x80\x00\x00", 8);
    SearchReplacePattern(dllBase, "\xC7\x45\xCC\x11\x00\x00\x00", "\xC7\x45\xCC\x11\x80\x00\x00", 7);
    SearchReplacePattern(dllBase, "\xC7\x45\xCC\xE0\x00\x00\x00", "\xC7\x45\xCC\xE0\x80\x00\x00", 7);
  }

  // Allow multiple instances
  if (config.GetInt(_T("AllowMultipleInstances"))) {
    // Patch FindWindowA import to always tell ISLE that no other ISLE window exists
    OverwriteImport(exeBase, "FindWindowA", (LPVOID)InterceptFindWindowA);
  }

  // Debug mode
  if (config.GetInt(_T("DebugToggle"))) {
    // LEGO1 uses a string pointer to know if OGEL has been typed, if the string pointer sees 0x0
    // (null-terminator/end of string), then debug mode is enabled. So we just replace the first
    // character with 0x0 and it's permanently on.
    SearchReplacePattern(dllBase, "OGEL", "\x0GEL", 4, TRUE);
  }





  // DDRAW GetSurfaceDesc Override
  OverwriteCall((LPVOID) ((UINT_PTR)dllBase+0xBA7D5), (LPVOID)InterceptSurfaceGetDesc);

  // Window size hack
  /*SearchReplacePattern(exeBase,
                       "\x80\x02\x00\x00\xE0\x01\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x80\x02\x00\x00\xE0\x01\x00\x00",
                       "\x40\x01\x00\x00\xE0\x01\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x80\x02\x00\x00\xE0\x01\x00\x00", 24);*/

  return 0;
}

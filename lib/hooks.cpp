#include "hooks.h"

#include <STDIO.H>

#include "../cmn/path.h"
#include "config.h"
#include "util.h"

HWND isleWindow = NULL;

void InterceptOutputDebugStringA(LPCSTR s)
{
  printf("%s\n", s);
  MessageBoxA(isleWindow, s, "LEGO Island sez", 0);
}

HWND WINAPI InterceptCreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
  // Grab a copy of the ISLE window so we can do stuff with it
  isleWindow = CreateWindowExA(dwExStyle, lpClassName, "LEGO Island: Rebuilt", dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
  return isleWindow;
}

HWND WINAPI InterceptFindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName)
{
  return NULL;
}

void ForceDDPixelFormatTo16(LPDDPIXELFORMAT lpDDPixelFormat)
{
  if (lpDDPixelFormat->dwRGBBitCount == 32) {
    lpDDPixelFormat->dwRGBBitCount = 16;
    lpDDPixelFormat->dwRBitMask = 0xF800;
    lpDDPixelFormat->dwGBitMask = 0x07E0;
    lpDDPixelFormat->dwBBitMask = 0x001F;
  }
}

void ForceDDSurfaceDescTo16(LPDDSURFACEDESC lpDDSurfaceDesc)
{
  DDPIXELFORMAT &pixfmt = lpDDSurfaceDesc->ddpfPixelFormat;
  if (pixfmt.dwRGBBitCount == 32) {
    // LEGO Island has no support for 32-bit surfaces, so we tell it we're on a 16-bit surface
    lpDDSurfaceDesc->lPitch /= 2;
    ForceDDPixelFormatTo16(&pixfmt);
  }
}

typedef HRESULT (WINAPI *ddSurfaceGetDescFunction)(LPDIRECTDRAWSURFACE lpDDSurface, LPDDSURFACEDESC lpDDSurfaceDesc);
ddSurfaceGetDescFunction originalDDSurfaceGetDescFunction = NULL;
HRESULT WINAPI InterceptSurfaceGetDesc(LPDIRECTDRAWSURFACE lpDDSurface, LPDDSURFACEDESC lpDDSurfaceDesc)
{
  HRESULT res = originalDDSurfaceGetDescFunction(lpDDSurface, lpDDSurfaceDesc);

  if (res == DD_OK) {
    ForceDDSurfaceDescTo16(lpDDSurfaceDesc);
  }

  return res;
}

typedef HRESULT (WINAPI *ddSurfaceGetPixelFormatFunction)(LPDIRECTDRAWSURFACE lpDDSurface, LPDDPIXELFORMAT lpDDPixelFormat);
ddSurfaceGetPixelFormatFunction originalDDSurfaceGetPixelFunction = NULL;
HRESULT WINAPI InterceptSurfaceGetPixelFormatFunction(LPDIRECTDRAWSURFACE lpDDSurface, LPDDPIXELFORMAT lpDDPixelFormat)
{
  HRESULT res = originalDDSurfaceGetPixelFunction(lpDDSurface, lpDDPixelFormat);

  if (res == DD_OK) {
    //ForceDDPixelFormatTo16(lpDDPixelFormat);
  }

  return res;
}

typedef HRESULT (WINAPI *ddCreateSurfaceFunction)(LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *unknown);
ddCreateSurfaceFunction originalCreateSurfaceFunction = NULL;
HRESULT WINAPI InterceptCreateSurface(LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *unknown)
{
  //ForceDDSurfaceDescTo16(lpDDSurfaceDesc);

  HRESULT res = originalCreateSurfaceFunction(lpDD, lpDDSurfaceDesc, lplpDDSurface, unknown);

  if (res == DD_OK) {
    if (!originalDDSurfaceGetPixelFunction) {
      originalDDSurfaceGetPixelFunction = (ddSurfaceGetPixelFormatFunction)OverwriteVirtualTable(*lplpDDSurface, 0x15, (LPVOID)InterceptSurfaceGetPixelFormatFunction);
    }

    if (!originalDDSurfaceGetDescFunction) {
      //originalDDSurfaceGetDescFunction = (ddSurfaceGetDescFunction)OverwriteVirtualTable(*lplpDDSurface, 0x16, (LPVOID)InterceptSurfaceGetDesc);
      originalDDSurfaceGetDescFunction = (ddSurfaceGetDescFunction)OverwriteVirtualTable(*lplpDDSurface, 0x16, NULL);
    }
  }

  return res;
}

typedef HRESULT (WINAPI *ddGetDisplayModeFunction)(LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc);
ddGetDisplayModeFunction originalGetDisplayMode = NULL;
HRESULT WINAPI InterceptGetDisplayMode(LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc)
{
  HRESULT res = originalGetDisplayMode(lpDD, lpDDSurfaceDesc);

  //ForceDDSurfaceDescTo16(lpDDSurfaceDesc);

  return res;
}

ddCreateFunction ddCreateOriginal = NULL;
HRESULT WINAPI InterceptDirectDrawCreate(GUID *lpGUID, LPDIRECTDRAW *lplpDD, IUnknown *pUnkOuter)
{
  HRESULT res = ddCreateOriginal(lpGUID, lplpDD, pUnkOuter);

  if (res == DD_OK) {
    if (!originalGetDisplayMode) {
      originalGetDisplayMode = (ddGetDisplayModeFunction)OverwriteVirtualTable(*lplpDD, 0xC, (LPVOID)InterceptGetDisplayMode);
    }

    ddCreateSurfaceFunction f = (ddCreateSurfaceFunction)OverwriteVirtualTable(*lplpDD, 0x6, (LPVOID)InterceptCreateSurface);
    if (f != InterceptCreateSurface) {
      originalCreateSurfaceFunction = f;
    }
  }

  return res;
}

void ReturnRegistryYESNOFromBool(LPBYTE lpData, BOOL value)
{
  strcpy((char*)lpData, value ? "YES" : "NO");
}

LONG WINAPI InterceptRegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
  if (!strcmp(lpValueName, "Music")) {

    // Music option
    ReturnRegistryYESNOFromBool(lpData, config.GetInt(_T("MusicToggle"), 1));
    return ERROR_SUCCESS;

  } else if (!strcmp(lpValueName, "UseJoystick")) {

    ReturnRegistryYESNOFromBool(lpData, config.GetInt(_T("UseJoystick"), 0));
    return ERROR_SUCCESS;

  } else if (!strcmp(lpValueName, "Full Screen")) {

    ReturnRegistryYESNOFromBool(lpData, config.GetInt(_T("FullScreen"), 1));
    return ERROR_SUCCESS;

  } else if (!strcmp(lpValueName, "Draw Cursor")) {

    ReturnRegistryYESNOFromBool(lpData, config.GetInt(_T("DrawCursor"), 1));
    return ERROR_SUCCESS;

  } else if (!strcmp(lpValueName, "savepath")) {

    // If enabled, return a safe %APPDATA% based save location rather than its default
    // "C:\Program Files" location
    if (config.GetInt(_T("RedirectSaveData"))) {
      // Generate directory
      TCHAR save_path[MAX_PATH];
      if (GetSafeLEGOIslandSavePath(save_path)) {
        strcpy((char*)lpData, save_path);
        return ERROR_SUCCESS;
      } else {
        MessageBoxA(isleWindow, "Failed to redirect save path. Default will be used instead.", 0, 0);
      }
    }

  } else if (!strcmp(lpValueName, "diskpath") || !strcmp(lpValueName, "cdpath")) {

    // Pass through

  } else {
    MessageBoxA(isleWindow, lpValueName, "ISLE asked for...", 0);
  }

  // Pass these through
  return RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

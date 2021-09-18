#include "hooks.h"

#include <STDIO.H>

#include "util.h"

void InterceptOutputDebugStringA(LPCSTR s)
{
  printf("%s\n", s);
  MessageBoxA(0,s,"LEGO Island sez",0);
}

HWND WINAPI InterceptCreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
  HWND window = CreateWindowExA(dwExStyle, lpClassName, "LEGO Island: Rebuilt", dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

  return window;
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

  if (res == DD_OK && !originalGetDisplayMode) {
    originalGetDisplayMode = (ddGetDisplayModeFunction)OverwriteVirtualTable(*lplpDD, 0xC, (LPVOID)InterceptGetDisplayMode);
    originalCreateSurfaceFunction = (ddCreateSurfaceFunction)OverwriteVirtualTable(*lplpDD, 0x6, (LPVOID)InterceptCreateSurface);
  }

  return res;
}

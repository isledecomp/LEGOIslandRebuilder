#include "hooks.h"

#include <MATH.H>
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
    //ForceDDSurfaceDescTo16(lpDDSurfaceDesc);
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

    //ReturnRegistryYESNOFromBool(lpData, config.GetInt(_T("FullScreen"), 1));
    ReturnRegistryYESNOFromBool(lpData, FALSE);
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

    printf("Passed through requested registry key \"%s\"\n", lpValueName);

  }

  // Pass these through
  return RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

BOOL StringEquals(const std::string &a, const char *b)
{
  return !(strcmp(a.c_str(), b));
}

void WINAPI InterceptSleep(DWORD dwMilliseconds)
{
  // Do nothing
  std::string fps_behavior = config.GetString(_T("FPSLimit"));

  // If uncapped, do nothing
  if (!StringEquals(fps_behavior, "Uncapped")) {
    // If not default, pass through new FPS
    if (StringEquals(fps_behavior, "Limited")) {
      dwMilliseconds = 1000.0f / config.GetFloat(_T("CustomFPS"));
    }

    Sleep(dwMilliseconds);
  }
}

/*typedef D3DVALUE (WINAPI *d3drmViewportGetFieldFunction)(LPDIRECT3DRMVIEWPORT viewport);
d3drmViewportGetFieldFunction d3drmViewportGetFieldOriginal = NULL;*/

LPDIRECT3DRMVIEWPORT last_viewport = NULL;
D3DVALUE last_fov = 0.0f;
typedef HRESULT (WINAPI *d3drmViewportSetFieldFunction)(LPDIRECT3DRMVIEWPORT viewport, D3DVALUE field);
d3drmViewportSetFieldFunction d3drmViewportSetFieldOriginal = NULL;
HRESULT WINAPI InterceptD3DRMViewportSetField(LPDIRECT3DRMVIEWPORT viewport, D3DVALUE field)
{
  last_viewport = viewport;
  last_fov = field;

  return d3drmViewportSetFieldOriginal(viewport, field);
}

typedef HRESULT (WINAPI *d3drmCreateViewportFunction)(LPDIRECT3DRM d3drm, LPDIRECT3DRMDEVICE device, LPDIRECT3DRMFRAME frame, DWORD x, DWORD y, DWORD w, DWORD h, LPDIRECT3DRMVIEWPORT *viewport);
d3drmCreateViewportFunction d3drmCreateViewportOriginal = NULL;
HRESULT WINAPI InterceptD3DRMCreateViewport(LPDIRECT3DRM d3drm, LPDIRECT3DRMDEVICE device, LPDIRECT3DRMFRAME frame, DWORD x, DWORD y, DWORD w, DWORD h, LPDIRECT3DRMVIEWPORT *viewport)
{
  HRESULT res = d3drmCreateViewportOriginal(d3drm, device, frame, x, y, w, h, viewport);

  if (res == DD_OK) {
    /*if (!d3drmViewportGetFieldOriginal) {
      d3drmViewportGetFieldOriginal = (d3drmViewportGetFieldFunction)OverwriteVirtualTable(*viewport, 0x22, NULL);
    }*/

    if (!d3drmViewportSetFieldOriginal) {
      d3drmViewportSetFieldOriginal = (d3drmViewportSetFieldFunction)OverwriteVirtualTable(*viewport, 0x10, (LPVOID)InterceptD3DRMViewportSetField);
    }
  }

  return res;
}

d3drmCreateFunction d3drmCreateOriginal = NULL;
HRESULT WINAPI InterceptDirect3DRMCreate(LPDIRECT3DRM FAR *lplpDirect3DRM)
{
  HRESULT result = d3drmCreateOriginal(lplpDirect3DRM);

  if (result == DD_OK) {
    if (!d3drmCreateViewportOriginal) {
      d3drmCreateViewportOriginal = (d3drmCreateViewportFunction)OverwriteVirtualTable(*lplpDirect3DRM, 0x38, (LPVOID)InterceptD3DRMCreateViewport);
    }
  }

  return result;
}

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif

#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

WNDPROC originalWndProc = NULL;
LRESULT CALLBACK InterceptWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (uMsg == WM_MOUSEWHEEL) {
    short distance = HIWORD(wParam);
    distance /= WHEEL_DELTA;

    float multiplier = 0.005 * distance;

    InterceptD3DRMViewportSetField(last_viewport, last_fov + multiplier);
  } else if (uMsg == WM_KEYDOWN) {
    if (wParam == 'M') {
      MessageBoxA(0, "would be cool to trigger any animation from here", 0, 0);
    }
  }
  return originalWndProc(hwnd, uMsg, wParam, lParam);
}

ATOM WINAPI InterceptRegisterClassA(const WNDCLASSA *c)
{
  WNDCLASSA copy = *c;
  originalWndProc = copy.lpfnWndProc;
  copy.lpfnWndProc = InterceptWindowProc;

  return RegisterClassA(&copy);
}

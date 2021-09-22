#ifndef HOOKS_H
#define HOOKS_H

#include <DDRAW.H>
#include <WINDOWS.H>

void InterceptOutputDebugStringA(LPCSTR s);

HWND WINAPI InterceptCreateWindowExA(
  DWORD          dwExStyle,
  LPCSTR         lpClassName,
  LPCSTR         lpWindowName,
  DWORD          dwStyle,
  int            X,
  int            Y,
  int            nWidth,
  int            nHeight,
  HWND           hWndParent,
  HMENU          hMenu,
  HINSTANCE      hInstance,
  LPVOID         lpParam
);

LONG
APIENTRY
InterceptRegQueryValueExA (
    HKEY hKey,
    LPCSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE lpData,
    LPDWORD lpcbData
);

HWND WINAPI InterceptFindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName);

typedef HRESULT (WINAPI *ddCreateFunction)(GUID *lpGUID, LPDIRECTDRAW *lplpDD, IUnknown *pUnkOuterS);
extern ddCreateFunction ddCreateOriginal;
HRESULT WINAPI InterceptDirectDrawCreate(GUID *lpGUID, LPDIRECTDRAW *lplpDD, IUnknown *pUnkOuter);

HRESULT WINAPI InterceptSurfaceGetDesc(LPDIRECTDRAWSURFACE lpDDSurface, LPDDSURFACEDESC lpDDSurfaceDesc);

#endif // HOOKS_H

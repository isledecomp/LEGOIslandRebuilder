#ifndef HOOKS_H
#define HOOKS_H

#include <D3DRM.H>
#include <DDRAW.H>
#include <DINPUT.H>
#include <DSOUND.H>
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

ATOM WINAPI InterceptRegisterClassA(const WNDCLASSA *c);

HWND WINAPI InterceptFindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName);

typedef HRESULT (WINAPI *ddCreateFunction)(GUID *lpGUID, LPDIRECTDRAW *lplpDD, IUnknown *pUnkOuterS);
extern ddCreateFunction ddCreateOriginal;
HRESULT WINAPI InterceptDirectDrawCreate(GUID *lpGUID, LPDIRECTDRAW *lplpDD, IUnknown *pUnkOuter);

HRESULT WINAPI InterceptSurfaceGetDesc(LPDIRECTDRAWSURFACE lpDDSurface, LPDDSURFACEDESC lpDDSurfaceDesc);

VOID WINAPI InterceptSleep(DWORD dwMilliseconds);

typedef HRESULT (WINAPI *d3drmCreateFunction)(LPDIRECT3DRM FAR *lplpDirect3DRM);
extern d3drmCreateFunction d3drmCreateOriginal;
HRESULT WINAPI InterceptDirect3DRMCreate(LPDIRECT3DRM FAR *lplpDirect3DRM);

typedef HRESULT (WINAPI *dsCreateFunction)(LPGUID lpGuid, LPDIRECTSOUND* ppDS, LPUNKNOWN  pUnkOuter );
extern dsCreateFunction dsCreateOriginal;
HRESULT WINAPI InterceptDirectSoundCreate(LPGUID lpGuid, LPDIRECTSOUND* ppDS, LPUNKNOWN  pUnkOuter );

SHORT WINAPI InterceptGetAsyncKeyState(int vKey);

typedef HRESULT (WINAPI *dinputCreateFunction)(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter);
extern dinputCreateFunction dinputCreateOriginal;
HRESULT WINAPI InterceptDirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter);

#endif // HOOKS_H
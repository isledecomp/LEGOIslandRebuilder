#ifndef HOOKS_H
#define HOOKS_H

#include <cstdio>

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

typedef _CRTIMP size_t (__cdecl *freadFunction)(void *buffer, size_t size, size_t count, FILE *stream);
extern freadFunction freadOriginal;
_CRTIMP size_t __cdecl InterceptFread(void *buffer, size_t size, size_t count, FILE *stream);

enum MxResult
{
  SUCCESS = 0,
  FAILURE = 0xFFFFFFFF
};

enum TransitionType
{
  NOT_TRANSITIONING = 0,
  NO_ANIMATION = 1,
  DISSOLVE = 2,
  MOSAIC = 3,
  WIPE_DOWN = 4,
  WINDOW = 5,
  BROKEN = 6
};

class MxTransitionManager
{
public:
  MxResult InterceptStartTransition(TransitionType animationType, int speed, byte unk, bool playMusicInTransition);
};

typedef MxResult (MxTransitionManager::* startTransitionFunction)(TransitionType, int, byte, bool);
extern startTransitionFunction startTransitionOriginal;

#endif // HOOKS_H

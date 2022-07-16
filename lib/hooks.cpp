#include "hooks.h"

#include <MAP>
#include <MATH.H>
#include <STDIO.H>
#include <VECTOR>

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
  // If full screen, hide the titlebar
  if (config.GetInt(_T("FullScreen"))) {
    dwStyle = WS_POPUP;
    dwExStyle = 0;
  }

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

typedef HRESULT (WINAPI *ddBltFunction)(LPDIRECTDRAWSURFACE lpSurface, LPRECT lpRectDest, LPDIRECTDRAWSURFACE lpSource, LPRECT lpRectSrc, DWORD dwFlags, LPDDBLTFX lpDDBltFx);
ddBltFunction originalBltFunction = NULL;
HRESULT WINAPI InterceptDirectDrawSurfaceBlt(LPDIRECTDRAWSURFACE lpDest, LPRECT lpRectDest, LPDIRECTDRAWSURFACE lpSource, LPRECT lpRectSrc, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
  return originalBltFunction(lpDest, lpRectDest, lpSource, lpRectSrc, dwFlags, lpDDBltFx);
}

typedef HRESULT (WINAPI *ddCreateSurfaceFunction)(LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *unknown);
ddCreateSurfaceFunction originalCreateSurfaceFunction = NULL;
HRESULT WINAPI InterceptCreateSurface(LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *unknown)
{
  HRESULT res = originalCreateSurfaceFunction(lpDD, lpDDSurfaceDesc, lplpDDSurface, unknown);

  if (res == DD_OK) {
    if (!originalDDSurfaceGetPixelFunction) {
      originalDDSurfaceGetPixelFunction = (ddSurfaceGetPixelFormatFunction)OverwriteVirtualTable(*lplpDDSurface, 0x15, (LPVOID)InterceptSurfaceGetPixelFormatFunction);
    }

    if (!originalDDSurfaceGetDescFunction) {
      originalDDSurfaceGetDescFunction = (ddSurfaceGetDescFunction)OverwriteVirtualTable(*lplpDDSurface, 0x16, (LPVOID)InterceptSurfaceGetDesc);
    }

    if (!originalBltFunction) {
      originalBltFunction = (ddBltFunction)OverwriteVirtualTable(*lplpDDSurface, 0x5, (LPVOID)InterceptDirectDrawSurfaceBlt);
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

typedef HRESULT (WINAPI *ddrawQueryInterfaceFunction) (LPDIRECTDRAW lpDD, REFIID riid, LPVOID FAR * ppvObj);
ddrawQueryInterfaceFunction ddrawQueryInterfaceOriginal = NULL;
HRESULT WINAPI InterceptDirectDrawQueryInterface(LPDIRECTDRAW lpDD, REFIID riid, LPVOID FAR * ppvObj)
{
  HRESULT res = ddrawQueryInterfaceOriginal(lpDD, riid, ppvObj);

  return res;
}

ddCreateFunction ddCreateOriginal = NULL;
HRESULT WINAPI InterceptDirectDrawCreate(GUID *lpGUID, LPDIRECTDRAW *lplpDD, IUnknown *pUnkOuter)
{
  HRESULT res = ddCreateOriginal(lpGUID, lplpDD, pUnkOuter);

  if (res == DD_OK) {
    ddGetDisplayModeFunction f1 = (ddGetDisplayModeFunction)OverwriteVirtualTable(*lplpDD, 0xC, (LPVOID)InterceptGetDisplayMode);
    if (InterceptGetDisplayMode != f1) {
      originalGetDisplayMode = f1;
    }

    ddCreateSurfaceFunction f2 = (ddCreateSurfaceFunction)OverwriteVirtualTable(*lplpDD, 0x6, (LPVOID)InterceptCreateSurface);
    if (InterceptCreateSurface != f2) {
      originalCreateSurfaceFunction = f2;
    }

    ddrawQueryInterfaceFunction f3 = (ddrawQueryInterfaceFunction)OverwriteVirtualTable(*lplpDD, 0x0, (LPVOID)InterceptDirectDrawQueryInterface);
    if (InterceptDirectDrawQueryInterface != f3) {
      ddrawQueryInterfaceOriginal = f3;
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

    ReturnRegistryYESNOFromBool(lpData, config.GetInt(_T("DrawCursor"), 0));
    return ERROR_SUCCESS;

  } else if (!strcmp(lpValueName, "Flip Surfaces")) {

    ReturnRegistryYESNOFromBool(lpData, config.GetInt(_T("FlipSurfaces"), 0));
    return ERROR_SUCCESS;

  } else if (!strcmp(lpValueName, "Back Buffers in Video RAM")) {

    ReturnRegistryYESNOFromBool(lpData, config.GetInt(_T("BackBuffersInVRAM"), 0));
    return ERROR_SUCCESS;

  } else if (!strcmp(lpValueName, "3D Device ID")) {

    std::string dev_id = config.GetString("D3DDeviceID");
    strcpy((char*)lpData, dev_id.c_str());
    return ERROR_SUCCESS;

  } else if (!strcmp(lpValueName, "3D Device Name")) {

    std::string dev_name = config.GetString("D3DDevice");
    strcpy((char*)lpData, dev_name.c_str());
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

void WINAPI InterceptSleep(DWORD dwMilliseconds)
{
  // If uncapped, do nothing. Otherwise pass through as normal.
  if (config.GetString(_T("FPSLimit")) != "Uncapped") {
    Sleep(dwMilliseconds);
  }
}

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

const SIZE_T max_frame_numbers = 10;
SIZE_T current_frame = 0;
DWORD frame_deltas[max_frame_numbers];
DWORD last_time = 0;

typedef HRESULT (WINAPI *d3drmDeviceUpdateFunction)(LPDIRECT3DRMDEVICE device);
d3drmDeviceUpdateFunction d3drmDeviceUpdateOriginal = NULL;
HRESULT WINAPI InterceptD3DRMDeviceUpdate(LPDIRECT3DRMDEVICE device)
{
  DWORD now = timeGetTime();

  if (last_time != 0) {
    DWORD diff = now - last_time;
    frame_deltas[current_frame%max_frame_numbers] = diff;
    current_frame++;

    if (current_frame >= max_frame_numbers) {
      double avg_delta = 0;
      for (size_t i=0; i<max_frame_numbers; i++) {
        avg_delta += frame_deltas[i];
      }
      avg_delta /= max_frame_numbers;
      //printf("Avg FPS: %f\n", 1000.0 / avg_delta);
    }
  }

  last_time = now;

  return d3drmDeviceUpdateOriginal(device);
}

std::map<LPDIRECT3DDEVICE, std::vector<D3DPICKRECORD> > g_PickRecords;

void MultiplyMatrix(D3DMATRIX &c, const D3DMATRIX &a, const D3DMATRIX &b)
{
  c._11 = (a._11 * b._11) + (a._21 * b._12) + (a._31 * b._13) + (a._41 * b._14);
  c._21 = (a._11 * b._21) + (a._21 * b._22) + (a._31 * b._23) + (a._41 * b._24);
  c._31 = (a._11 * b._31) + (a._21 * b._32) + (a._31 * b._33) + (a._41 * b._34);
  c._41 = (a._11 * b._41) + (a._21 * b._42) + (a._31 * b._43) + (a._41 * b._44);

  c._12 = (a._12 * b._11) + (a._22 * b._12) + (a._32 * b._13) + (a._42 * b._14);
  c._22 = (a._12 * b._21) + (a._22 * b._22) + (a._32 * b._23) + (a._42 * b._24);
  c._32 = (a._12 * b._31) + (a._22 * b._32) + (a._32 * b._33) + (a._42 * b._34);
  c._42 = (a._12 * b._41) + (a._22 * b._42) + (a._32 * b._43) + (a._42 * b._44);

  c._13 = (a._13 * b._11) + (a._23 * b._12) + (a._33 * b._13) + (a._43 * b._14);
  c._23 = (a._13 * b._21) + (a._23 * b._22) + (a._33 * b._23) + (a._43 * b._24);
  c._33 = (a._13 * b._31) + (a._23 * b._32) + (a._33 * b._33) + (a._43 * b._34);
  c._43 = (a._13 * b._41) + (a._23 * b._42) + (a._33 * b._43) + (a._43 * b._44);

  c._14 = (a._14 * b._11) + (a._24 * b._12) + (a._34 * b._13) + (a._44 * b._14);
  c._24 = (a._14 * b._21) + (a._24 * b._22) + (a._34 * b._23) + (a._44 * b._24);
  c._34 = (a._14 * b._31) + (a._24 * b._32) + (a._34 * b._33) + (a._44 * b._34);
  c._44 = (a._14 * b._41) + (a._24 * b._42) + (a._34 * b._43) + (a._44 * b._44);
}

typedef HRESULT (WINAPI *d3dimPickFunction) (LPDIRECT3DDEVICE, LPDIRECT3DEXECUTEBUFFER, LPDIRECT3DVIEWPORT, DWORD, LPD3DRECT);
d3dimPickFunction d3dimPickOriginal = NULL;
HRESULT WINAPI InterceptD3DIMPick(LPDIRECT3DDEVICE lpDevice, LPDIRECT3DEXECUTEBUFFER lpBuffer, LPDIRECT3DVIEWPORT lpViewport, DWORD, LPD3DRECT lpRect)
{
  D3DEXECUTEBUFFERDESC desc;
  ZeroMemory(&desc, sizeof(D3DEXECUTEBUFFERDESC));
  desc.dwSize = sizeof(D3DEXECUTEBUFFERDESC);

  HRESULT hr;

  hr = lpBuffer->Lock(&desc);
  if (hr != D3D_OK) {
    return hr;
  }

  D3DEXECUTEDATA data;
  hr = lpBuffer->GetExecuteData(&data);
  if (hr != D3D_OK) {
    return hr;
  }

  char *instr = (char *)desc.lpData + data.dwInstructionOffset;
  D3DVERTEX *vertex = (D3DVERTEX *) ((char *)desc.lpData + data.dwVertexOffset);
  std::vector<D3DVERTEX> dst_buf;
  dst_buf.resize(data.dwVertexCount);

  std::vector<D3DPICKRECORD> &records = g_PickRecords[lpDevice];
  records.clear();

  LPD3DINSTRUCTION current;
  while (current = (LPD3DINSTRUCTION)instr, current->bOpcode != D3DOP_EXIT) {
    BYTE size = current->bSize;
    WORD count = current->wCount;

    instr += sizeof(D3DINSTRUCTION);

    switch (current->bOpcode) {
    case D3DOP_TRIANGLE:
    {
      for (WORD i=0; i<count; i++) {
        D3DTRIANGLE *ci = (D3DTRIANGLE*)instr;

        bool inside_x = lpRect->x1 >= min(dst_buf[ci->v1].x, min(dst_buf[ci->v2].x, dst_buf[ci->v3].x)) && lpRect->x1 <= max(dst_buf[ci->v1].x, max(dst_buf[ci->v2].x, dst_buf[ci->v3].x));
        bool inside_y = lpRect->y1 >= min(dst_buf[ci->v1].y, min(dst_buf[ci->v2].y, dst_buf[ci->v3].y)) && lpRect->y1 <= max(dst_buf[ci->v1].y, max(dst_buf[ci->v2].y, dst_buf[ci->v3].y));

        if (inside_x && inside_y) {
          D3DPICKRECORD record;

          record.bOpcode = current->bOpcode;
          record.bPad = 0;

          // Write current instruction offset into file
          record.dwOffset = (DWORD)((LPBYTE)current + sizeof(D3DINSTRUCTION) - ((LPBYTE)desc.lpData + data.dwInstructionOffset));

          // Just get the center Z
          record.dvZ = (vertex[ci->v1].z + vertex[ci->v2].z + vertex[ci->v3].z) / 3;

          if (records.empty()) {
            records.push_back(record);
          } else if (record.dvZ > records[0].dvZ) {
            records[0] = record;
          }

          printf("Clicked  %u,%u  %f  to  %f,%f  %f,%f  %f,%f\n",
            lpRect->x1, lpRect->y1, record.dvZ,
            dst_buf[ci->v1].x, dst_buf[ci->v1].y,
            dst_buf[ci->v2].x, dst_buf[ci->v2].y,
            dst_buf[ci->v3].x, dst_buf[ci->v3].y
          );
        } else {
          /*printf("Comparing  %u,%u  to  %f,%f  %f,%f  %f,%f  failed\n",
            lpRect->x1, lpRect->y1,
            dst_buf[ci->v1].x, dst_buf[ci->v1].y,
            dst_buf[ci->v2].x, dst_buf[ci->v2].y,
            dst_buf[ci->v3].x, dst_buf[ci->v3].y
          );*/
        }

        instr += size;
      }
      break;
    }
    case D3DOP_BRANCHFORWARD:
    {
      for (int i = 0; i < count; ++i) {
        D3DBRANCH *ci = (D3DBRANCH *)instr;

        if ((data.dsStatus.dwStatus & ci->dwMask) == ci->dwValue) {
          if (!ci->bNegate) {
            if (ci->dwOffset) {
              instr = (char*)current + ci->dwOffset;
              break;
            }
          }
        } else {
          if (ci->bNegate) {
            if (ci->dwOffset) {
              instr = (char*)current + ci->dwOffset;
              break;
            }
          }
        }

        instr += size;
      }
      break;
    }
    case D3DOP_PROCESSVERTICES:
    {
      LPDIRECT3DDEVICE2 d3dev2 = NULL;
      lpDevice->QueryInterface(IID_IDirect3DDevice2, (void**) &d3dev2);

      for (int i = 0; i < count; ++i) {
        D3DPROCESSVERTICES *ci = (D3DPROCESSVERTICES*)instr;
        DWORD op = ci->dwFlags & D3DPROCESSVERTICES_OPMASK;

        switch (op) {
        case D3DPROCESSVERTICES_TRANSFORMLIGHT:
        case D3DPROCESSVERTICES_TRANSFORM:
        {
          D3DVIEWPORT vp;
          ZeroMemory(&vp, sizeof(D3DVIEWPORT));
          vp.dwSize = sizeof(D3DVIEWPORT);
          lpViewport->GetViewport(&vp);

          D3DMATRIX view, proj, world, mat;

          d3dev2->GetTransform(D3DTRANSFORMSTATE_VIEW, &view);
          d3dev2->GetTransform(D3DTRANSFORMSTATE_PROJECTION, &proj);
          d3dev2->GetTransform(D3DTRANSFORMSTATE_WORLD, &world);

          MultiplyMatrix(mat, view, world);
          MultiplyMatrix(mat, proj, mat);

          for (DWORD j=0; j<ci->dwCount; j++) {
            D3DVERTEX in = vertex[ci->wStart+j];

            float x, y, z, rhw;

            x   = (in.x * mat._11) + (in.y * mat._21) + (in.z * mat._31) + mat._41;
            y   = (in.x * mat._12) + (in.y * mat._22) + (in.z * mat._32) + mat._42;
            z   = (in.x * mat._13) + (in.y * mat._23) + (in.z * mat._33) + mat._43;
            rhw = (in.x * mat._14) + (in.y * mat._24) + (in.z * mat._34) + mat._44;

            x /= rhw;
            y /= rhw;
            z /= rhw;

            y *= -1;

            x *= vp.dwWidth / 2;
            y *= vp.dwHeight / 2;
            z *= vp.dvMaxZ - vp.dvMinZ;

            x += vp.dwWidth / 2 + vp.dwX;
            y += vp.dwHeight / 2 + vp.dwY;
            z += vp.dvMinZ;

            D3DVERTEX &out = dst_buf[ci->wDest+j];
            out = in;
            out.x = x;
            out.y = y;
            out.z = z;
          }
          break;
        }
        default:
          MessageBoxA(0,"unhandled processvertices op",0,0);
          break;
        }

        instr += size;
      }
      break;
    }
    default:
      printf("unhandled op: %lx\n", current->bOpcode);
      instr += size * count;
      break;
    }
  }

  lpBuffer->Unlock();

  return D3D_OK;
}

typedef HRESULT (WINAPI *d3dimGetPickRecordsFunction)(LPDIRECT3DDEVICE lpDevice, LPDWORD dwCount, LPD3DPICKRECORD lpRecords);
d3dimGetPickRecordsFunction d3dimGetPickRecordsOriginal = NULL;
HRESULT WINAPI InterceptD3DIMGetPickRecords(LPDIRECT3DDEVICE lpDevice, LPDWORD dwCount, LPD3DPICKRECORD lpRecords)
{
  // Catch NULL count value
  if (!dwCount) {
    return DDERR_INVALIDPARAMS;
  }

  std::vector<D3DPICKRECORD> &records = g_PickRecords[lpDevice];
  if (lpRecords && *dwCount >= records.size()) {
    memcpy(lpRecords, &records[0], records.size() * sizeof(D3DPICKRECORD));
  }
  *dwCount = records.size();

  return D3D_OK;
}

LPDIRECT3DRMDEVICE d3drm_device = NULL;

typedef HRESULT (WINAPI *d3drmCreateViewportFunction)(LPDIRECT3DRM d3drm, LPDIRECT3DRMDEVICE device, LPDIRECT3DRMFRAME frame, DWORD x, DWORD y, DWORD w, DWORD h, LPDIRECT3DRMVIEWPORT *viewport);
d3drmCreateViewportFunction d3drmCreateViewportOriginal = NULL;
HRESULT WINAPI InterceptD3DRMCreateViewport(LPDIRECT3DRM d3drm, LPDIRECT3DRMDEVICE device, LPDIRECT3DRMFRAME frame, DWORD x, DWORD y, DWORD w, DWORD h, LPDIRECT3DRMVIEWPORT *viewport)
{
  HRESULT res = d3drmCreateViewportOriginal(d3drm, device, frame, x, y, w, h, viewport);

  d3drm_device = device;

  // Check if we're running on Wine, in which case we'll override Pick() and GetPickRecords()
  if (HMODULE ntdll = GetModuleHandle("NTDLL.DLL")) {
    if (GetProcAddress(ntdll, "wine_get_version")) {
      // We're officially running on Wine
      if (!d3dimPickOriginal) {
        LPDIRECT3DDEVICE d3dim_device;
        if (d3drm_device->GetDirect3DDevice(&d3dim_device) == D3D_OK) {
          d3dimPickOriginal = (d3dimPickFunction)OverwriteVirtualTable(d3dim_device, 12, (LPVOID)InterceptD3DIMPick);
          d3dimGetPickRecordsOriginal = (d3dimGetPickRecordsFunction)OverwriteVirtualTable(d3dim_device, 13, (LPVOID)InterceptD3DIMGetPickRecords);
        }
      }
    }
  }

  if (res == DD_OK) {
    if (!d3drmDeviceUpdateOriginal) {
      d3drmDeviceUpdateOriginal = (d3drmDeviceUpdateFunction)OverwriteVirtualTable(device, 14, (LPVOID)InterceptD3DRMDeviceUpdate);
    }

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
    d3drmCreateViewportFunction f1 = (d3drmCreateViewportFunction)OverwriteVirtualTable(*lplpDirect3DRM, 0x38, (LPVOID)InterceptD3DRMCreateViewport);
    if (f1 != InterceptD3DRMCreateViewport) {
      d3drmCreateViewportOriginal = f1;
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
    SHORT distance = HIWORD(wParam);
    distance /= WHEEL_DELTA;

    float multiplier = 0.005 * distance;

    InterceptD3DRMViewportSetField(last_viewport, last_fov + multiplier);

    return 0;
  } else if (uMsg == WM_KEYDOWN) {
    if (wParam == '5') {
      d3drm_device->SetQuality(D3DRMRENDER_WIREFRAME);
      printf("quality should have changed\n");
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

typedef HRESULT (WINAPI *dsCreateSoundBufferFunction)(LPDIRECTSOUND lpDS, LPCDSBUFFERDESC desc, LPDIRECTSOUNDBUFFER *buffer, LPUNKNOWN unk);
dsCreateSoundBufferFunction dsCreateSoundBufferOriginal = NULL;
HRESULT WINAPI InterceptDirectSoundCreateSoundBuffer(LPDIRECTSOUND lpDS, LPCDSBUFFERDESC desc, LPDIRECTSOUNDBUFFER *buffer, LPUNKNOWN unk)
{
  DSBUFFERDESC copy = *desc;

  if (config.GetInt(_T("StayActiveWhenDefocused")) && !(copy.dwFlags & DSBCAPS_PRIMARYBUFFER)) {
    copy.dwFlags |= DSBCAPS_GLOBALFOCUS;
  }

  return dsCreateSoundBufferOriginal(lpDS, &copy, buffer, unk);
}

dsCreateFunction dsCreateOriginal = NULL;
HRESULT WINAPI InterceptDirectSoundCreate(LPGUID lpGuid, LPDIRECTSOUND* ppDS, LPUNKNOWN  pUnkOuter )
{
  HRESULT res = dsCreateOriginal(lpGuid, ppDS, pUnkOuter);

  if (res == DD_OK) {
    if (!dsCreateSoundBufferOriginal) {
      dsCreateSoundBufferOriginal = (dsCreateSoundBufferFunction)OverwriteVirtualTable(*ppDS, 0x3, (LPVOID)InterceptDirectSoundCreateSoundBuffer);
    }
  }

  return res;
}

SHORT WINAPI InterceptGetAsyncKeyState(int vKey)
{
  if (config.GetInt("UseWASD")) {
    switch (vKey) {
    case VK_UP:
      vKey = 'W';
      break;
    case VK_LEFT:
      vKey = 'A';
      break;
    case VK_DOWN:
      vKey = 'S';
      break;
    case VK_RIGHT:
      vKey = 'D';
      break;
    case 'W':
      vKey = VK_UP;
      break;
    case 'A':
      vKey = VK_LEFT;
      break;
    case 'S':
      vKey = VK_DOWN;
      break;
    case 'D':
      vKey = VK_RIGHT;
      break;
    }
  }

  return GetAsyncKeyState(vKey);
}

typedef HRESULT (WINAPI *dinputGetDeviceStateFunction)(LPDIRECTINPUTDEVICEA lpDevice, DWORD cbData, LPVOID lpvData);
dinputGetDeviceStateFunction dinputGetDeviceStateOriginal = NULL;
HRESULT WINAPI InterceptDirectInputGetDeviceStateA(LPDIRECTINPUTDEVICEA lpDevice, DWORD cbData, LPVOID lpvData)
{
  HRESULT res = dinputGetDeviceStateOriginal(lpDevice, cbData, lpvData);

  if (config.GetInt("UseWASD") && res == DD_OK) {
    if (cbData == 256) {
      unsigned char *keys = (unsigned char *)lpvData;

      // Swap state of WASD with arrow keys
      std::swap(keys[DIK_W], keys[DIK_UP]);
      std::swap(keys[DIK_A], keys[DIK_LEFT]);
      std::swap(keys[DIK_S], keys[DIK_DOWN]);
      std::swap(keys[DIK_D], keys[DIK_RIGHT]);
    }
  }

  return res;
}

typedef HRESULT (WINAPI *dinputCreateDeviceFunction)(LPDIRECTINPUTA lpDI, REFGUID, LPDIRECTINPUTDEVICEA *, LPUNKNOWN);
dinputCreateDeviceFunction dinputCreateDeviceOriginal = NULL;
HRESULT WINAPI InterceptDirectInputCreateDeviceA(LPDIRECTINPUTA lpDI, REFGUID guid, LPDIRECTINPUTDEVICEA *ppDevice, LPUNKNOWN unk)
{
  HRESULT res = dinputCreateDeviceOriginal(lpDI, guid, ppDevice, unk);

  if (res == DD_OK) {
    if (!dinputGetDeviceStateOriginal) {
      dinputGetDeviceStateOriginal = (dinputGetDeviceStateFunction)OverwriteVirtualTable(*ppDevice, 0x9, (LPVOID)InterceptDirectInputGetDeviceStateA);
    }
  }

  return res;
}

dinputCreateFunction dinputCreateOriginal = NULL;
HRESULT WINAPI InterceptDirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter)
{
  HRESULT res = dinputCreateOriginal(hinst, dwVersion, ppDI, punkOuter);

  if (res == DD_OK) {
    if (!dinputCreateDeviceOriginal) {
      dinputCreateDeviceOriginal = (dinputCreateDeviceFunction)OverwriteVirtualTable(*ppDI, 0x3, (LPVOID)InterceptDirectInputCreateDeviceA);
    }
  }

  return res;
}

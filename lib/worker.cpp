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
  OverwriteImport(exeBase, "RegisterClassA", (LPVOID)InterceptRegisterClassA);
  OverwriteImport(dllBase, "Sleep", (LPVOID)InterceptSleep);
  OverwriteImport(exeBase, "Sleep", (LPVOID)InterceptSleep);
  OverwriteImport(dllBase, "GetAsyncKeyState", (LPVOID)InterceptGetAsyncKeyState);
  ddCreateOriginal = (ddCreateFunction)OverwriteImport(dllBase, "DirectDrawCreate", (LPVOID)InterceptDirectDrawCreate);
  d3drmCreateOriginal = (d3drmCreateFunction)OverwriteImport(dllBase, "Direct3DRMCreate", (LPVOID)InterceptDirect3DRMCreate);
  dsCreateOriginal = (dsCreateFunction)OverwriteImport(dllBase, "DirectSoundCreate", (LPVOID)InterceptDirectSoundCreate);
  dinputCreateOriginal = (dinputCreateFunction)OverwriteImport(dllBase, "DirectInputCreateA", (LPVOID)InterceptDirectInputCreateA);

  // Stay active when defocused
  if (config.GetInt(_T("StayActiveWhenDefocused"))) {
    // Patch jump if window isn't active (TODO: Replace with C++ patch)
    SearchReplacePattern(exeBase, "\x89\x58\x70", "\x90\x90\x90", 3);
  }

  // Allow multiple instances
  if (config.GetInt(_T("MultipleInstances"))) {
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

  // Patch navigation
  {
    const int nav_block_sz = 0x30;
    const char *nav_block_src = "\x28\x00\x00\x00\x6F\x12\x83\x3A\x00\x00\x20\x42\x00\x00\xA0\x41\x00\x00\x70\x41\x00\x00\xF0\x41\x00\x00\x80\x40\x00\x00\x70\x41\x00\x00\x48\x42\x00\x00\x48\x42\xCD\xCC\xCC\x3E\x00\x00\x00\x00";
    char nav_block_dst[nav_block_sz];
    memcpy(nav_block_dst, nav_block_src, nav_block_sz);

    UINT32 mouse_deadzone = config.GetInt(_T("MouseDeadzone"), 40);
    memcpy(nav_block_dst+0x0, &mouse_deadzone, sizeof(mouse_deadzone));

    float movement_max_spd = config.GetFloat(_T("MovementMaxSpeed"), 40.0f);
    memcpy(nav_block_dst+0x8, &movement_max_spd, sizeof(movement_max_spd));

    float turn_max_spd = config.GetFloat(_T("TurnMaxSpeed"), 20.0f);
    memcpy(nav_block_dst+0xC, &turn_max_spd, sizeof(turn_max_spd));

    float movement_max_accel = config.GetFloat(_T("MovementMaxAcceleration"), 15.0f);
    memcpy(nav_block_dst+0x10, &movement_max_accel, sizeof(movement_max_accel));

    float turn_max_accel = config.GetFloat(_T("TurnMaxAcceleration"), 30.0f);
    memcpy(nav_block_dst+0x14, &turn_max_accel, sizeof(turn_max_accel));

    float movement_min_accel = config.GetFloat(_T("MovementMinAcceleration"), 4.0f);
    memcpy(nav_block_dst+0x18, &movement_min_accel, sizeof(movement_min_accel));

    float turn_min_accel = config.GetFloat(_T("TurnMinAcceleration"), 15.0f);
    memcpy(nav_block_dst+0x1C, &turn_min_accel, sizeof(turn_min_accel));

    float movement_decel = config.GetFloat(_T("MovementDeceleration"), 50.0f);
    memcpy(nav_block_dst+0x20, &movement_decel, sizeof(movement_decel));

    float turn_decel = config.GetFloat(_T("TurnDeceleration"), 50.0f);
    memcpy(nav_block_dst+0x24, &turn_decel, sizeof(turn_decel));

    UINT32 turn_use_velocity = config.GetInt(_T("TurnUseVelocity"), FALSE);
    memcpy(nav_block_dst+0x2C, &turn_use_velocity, sizeof(turn_use_velocity));

    SearchReplacePattern(dllBase, nav_block_src, nav_block_dst, nav_block_sz);
  }

  // Model Quality
  std::string model_quality = config.GetString("ModelQuality");
  float mq_val = 3.6f;
  if (model_quality == "Infinite") {
    mq_val = 999999.0f;
  } else if (model_quality == "High") {
    mq_val = 5.0f;
  } else if (model_quality == "Medium") {
    mq_val = 3.6f;
  } else if (model_quality == "Low") {
    mq_val = 0.0f;
  }
  const char *mq_pattern = "\x00\x00\x80\x40\x66\x66\x66\x40";
  char mq_replace[8];
  memcpy(mq_replace, mq_pattern, 8);
  memcpy(mq_replace+4, &mq_val, sizeof(mq_val));
  SearchReplacePattern(dllBase, mq_pattern, mq_replace, 8);

  // Field of view
  const char *fov_pattern = "\x00\x00\x00\x3F\x17\x6C\xC1\x16\x6C\xC1\x76\x3F";
  char fov_replace[12];
  float fov;
  memcpy(fov_replace, fov_pattern, 12);         // Make editable copy of pattern
  memcpy(&fov, fov_replace, sizeof(fov));       // Get float from bytes
  fov *= 1.0f/config.GetFloat(_T("FOVMultiplier"));  // Multiply FOV
  memcpy(fov_replace, &fov, sizeof(fov));       // Store back into bytes
  SearchReplacePattern(dllBase, fov_pattern, fov_replace, 12);

  // DDRAW GetSurfaceDesc Override
  OverwriteCall((LPVOID) ((UINT_PTR)dllBase+0xBA7D5), (LPVOID)InterceptSurfaceGetDesc);

  // Window size hack
  /*SearchReplacePattern(exeBase,
                       "\x80\x02\x00\x00\xE0\x01\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x80\x02\x00\x00\xE0\x01\x00\x00",
                       "\x40\x01\x00\x00\xE0\x01\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x80\x02\x00\x00\xE0\x01\x00\x00", 24);*/

  return 0;
}

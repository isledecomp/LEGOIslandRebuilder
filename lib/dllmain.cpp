#include <IOSTREAM>
#include <TCHAR.H>
#include <VECTOR>
#include <WINDOWS.H>

#include "config.h"
#include "hooks.h"
#include "util.h"

__declspec(dllexport) DWORD WINAPI Patch()
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

  // Flip surfaces is incompatible with full screen, if these options are set, warn the user
  if (config.GetInt(_T("FlipSurfaces")) && !config.GetInt(_T("FullScreen"))) {
    if (MessageBoxA(0, "The setting 'Flip Video Memory Pages' is incompatible with LEGO Island's windowed mode. "
                       "LEGO Island will likely fail to start up unless you disable 'Flip Video Memory Pages' "
                       "or run in full screen mode. Do you wish to continue?", "Warning", MB_YESNO) == IDNO) {
      TerminateProcess(GetCurrentProcess(), 0);
    }
  }

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

  // Speed up startup
  if (config.GetInt(_T("SpeedUpStartUp"))) {
    // Replace "200" frame wait value with "1"
    const char *speedup_pattern = "\xC8\x00\x00\x00\x00\x00\x00\x00";
    const char *speedup_replace = "\x01\x00\x00\x00\x00\x00\x00\x00";

    SearchReplacePattern(exeBase, speedup_pattern, speedup_replace, 8, TRUE);
  }

  // Debug mode
  if (config.GetInt(_T("DebugToggle"))) {
    // LEGO1 uses a string pointer to know if OGEL has been typed, if the string pointer sees 0x0
    // (null-terminator/end of string), then debug mode is enabled. So we just replace the first
    // character with 0x0 and it's permanently on.
    SearchReplacePattern(dllBase, "OGEL", "\x0GEL", 4, TRUE);
  }

  // If using WASD, swap debug keys
  if (config.GetInt(_T("UseWASD"))) {
    const char *dbg_map_pattern = "\x18\x18\x18\x18\x18\x18\x18\x18\x18\x18\x18\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x18\x18\x18\x18\x18\x18\x18\x02\x18\x03\x04\x18\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x18\x0E\x18\x18\x0F\x18";
    const char *dbg_map_replace = "\x02\x18\x04\x0F\x18\x18\x18\x18\x18\x18\x18\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x18\x18\x18\x18\x18\x18\x18\x18\x18\x03\x18\x18\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x18\x0E\x18\x18\x18\x18";
    SearchReplacePattern(dllBase, dbg_map_pattern, dbg_map_replace, 48, TRUE);
  }

  // Exit crash fix
  if (config.GetInt(_T("ExitCrashFix"))) {
    const char *buffer_pattern = "\x50\x52\x51\x8B\x01\xFF\x50\x2C\x85\xC0\x75\x67\x8B\x7C\x24\x14\x8B\x74\x24\x30\x8B\xCB\xC1\xE9\x02\xF3\xA5\x8B\xCB\x83\xE1\x03\xF3\xA4\x8B\x55\x60";
    const char *buffer_replace = "\x53\x52\x51\x8B\x01\xFF\x50\x2C\x85\xC0\x75\x67\x8B\x7C\x24\x14\x8B\x74\x24\x30\x8B\x55\x60\x3B\xD3\x7C\x04\x8B\xCB\xEB\x02\x8B\xCA\x90\x90\xF3\xA4";

    const char *wndproc_pattern = "\xC6\x44\x24\x00\x00\x53\x56\x57";
    const char *wndproc_replace = "\xC6\x44\x24\x00\x0A\x53\x56\x57";

    SearchReplacePattern(dllBase, buffer_pattern, buffer_replace, 37, TRUE);
    SearchReplacePattern(exeBase, wndproc_pattern, wndproc_replace, 8, TRUE);
  }

  // Disable SMK scaling
  if (config.GetInt(_T("NativeSMK"))) {
    const char *smk_pattern = "\x8A\x4C\x24\x0C\x51";
    const char *smk_replace = "\xB1\x00\x90\x90\x51";
    SearchReplacePattern(dllBase, smk_pattern, smk_replace, 5, TRUE);
  }

  // Disable auto-finish in build sections
  if (config.GetInt(_T("DisableAutoFinishBuilding"))) {
    // Pattern used in August build (jump is much shorter so it uses a different opcode)
    const char *autofinish_pattern = "\x66\x39\x90\xBE\x00\x00\x00\x75";
    const char *autofinish_replace = "\x66\x39\x90\xBE\x00\x00\x00\xEB";

    if (SearchReplacePattern(dllBase, autofinish_pattern, autofinish_replace, 8) == 0) {
      // Pattern used in September build (jump is much longer)
      autofinish_pattern = "\x66\x39\x90\xBE\x00\x00\x00\x0F\x85\x86\x00\x00\x00";
      autofinish_replace = "\x66\x39\x90\xBE\x00\x00\x00\xE9\x87\x00\x00\x00\x90";
      SearchReplacePattern(dllBase, autofinish_pattern, autofinish_replace, 13);
    }
  }

  // Unhook turn speed
  float turn_max_spd = config.GetFloat(_T("TurnMaxSpeed"), 20.0f);

  if (config.GetInt(_T("UnhookTurnSpeed"))) {
    LPVOID code_offset = SearchPattern(dllBase, "\x74\x26\xD9\x46\x34", 5);
    if (code_offset) {
      const char *new_code = "\xD9\x46\x24\xD8\x4C\x24\x14\xD8\x4E\x34\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";
      WriteMemory(code_offset, (LPVOID)new_code, 36);
      turn_max_spd *= 2.0f;
    } else {
      printf("Failed to find code for unhooking turn speed\n");
    }
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

    // Value retrieved above
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

  // Transition Animation
  LPCVOID start_transition_offset = SearchPattern(dllBase, "\x89\x46\x2C\x8A\x44\x24\x14\x32\xC1\x24\x01\x32\xC1", 13);
  
  MxResult (MxTransitionManager::* pFunc)(TransitionType, int, byte, bool) = &MxTransitionManager::InterceptStartTransition;
  OverwriteAllCalls(dllBase, (char*)start_transition_offset - 40, (void*&) pFunc);
  
  startTransitionOriginal = PointerToMemberFunction<startTransitionFunction>((char*)start_transition_offset - 40);


  // Field of view
  const char *fov_pattern = "\x00\x00\x00\x3F\x17\x6C\xC1\x16\x6C\xC1\x76\x3F";
  char fov_replace[12];
  float fov;
  memcpy(fov_replace, fov_pattern, 12);         // Make editable copy of pattern
  memcpy(&fov, fov_replace, sizeof(fov));       // Get float from bytes
  fov *= 1.0f/config.GetFloat(_T("FOVMultiplier"));  // Multiply FOV
  memcpy(fov_replace, &fov, sizeof(fov));       // Store back into bytes
  SearchReplacePattern(dllBase, fov_pattern, fov_replace, 12);

  // FPS Cap
  std::string fps_behavior = config.GetString(_T("FPSLimit"));
  if (fps_behavior != "Default") {
    UINT32 frame_delta;

    if (fps_behavior == "Limited") {
      frame_delta = 1000.0f / config.GetFloat(_T("CustomFPS"));
    } else {
      frame_delta = 0;
    }

    WriteMemory((char*)exeBase+0x10B4, &frame_delta, sizeof(UINT32));
  }

  // Window size hack
  /*SearchReplacePattern(exeBase,
                       "\x80\x02\x00\x00\xE0\x01\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x80\x02\x00\x00\xE0\x01\x00\x00",
                       "\x40\x01\x00\x00\xE0\x01\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x80\x02\x00\x00\xE0\x01\x00\x00", 24);*/

  // Buffer overflow fix
  LPVOID fread_offset = SearchPattern(dllBase, "\x24\x10\x6A\x01\x50\x51", 6);
  freadOriginal = (freadFunction)OverwriteCall((char*)fread_offset + 6, (LPVOID)InterceptFread);

  return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH:
    // Allocate console
    AllocConsole();

    // Direct stdin/stderr/stdout to console
    _tfreopen(TEXT("CONIN$"), TEXT("r"), stdin);
    _tfreopen(TEXT("CONOUT$"), TEXT("w"), stderr);
    _tfreopen(TEXT("CONOUT$"), TEXT("w"), stdout);

    // Print success line
    printf("Injected successfully\n");

    Patch();
    break;
  }

  return TRUE;
}

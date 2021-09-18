#include <IOSTREAM>
#include <TCHAR.H>
#include <WINDOWS.H>

#include "worker.h"

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

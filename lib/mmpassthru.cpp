#include <WINDOWS.H>

extern "C" __declspec(dllexport) DWORD rbldGetTime()
{
  return timeGetTime();
}

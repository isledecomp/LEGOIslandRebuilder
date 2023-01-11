#include "util.h"

#include <STDIO.H>

BOOL WriteMemory(LPVOID destination, LPVOID source, size_t length, LPVOID oldData)
{
  DWORD oldProtec;

  if (VirtualProtect(destination, length, PAGE_EXECUTE_READWRITE, &oldProtec)) {
    // Read data out if requested
    if (oldData) {
      memcpy(oldData, destination, length);
    }

    // Write data if provided
    if (source) {
      memcpy(destination, source, length);
    }

    // Restore original protection
    VirtualProtect(destination, length, oldProtec, &oldProtec);

    return TRUE;
  } else {
    return FALSE;
  }
}

SIZE_T SearchReplacePattern(LPVOID imageBase, LPCVOID search, LPCVOID replace, SIZE_T count, BOOL only_once)
{
  SIZE_T instances = 0;

  HANDLE process = GetCurrentProcess();

  MEMORY_BASIC_INFORMATION mbi = {0};

  // Loop through memory pages
  UINT_PTR addr = (UINT_PTR)imageBase;
  while ((!instances || !only_once) && VirtualQueryEx(process, (LPVOID)addr, &mbi, sizeof(mbi)) && mbi.AllocationBase == imageBase) {
    if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS) {
      DWORD oldProtec;

      // Try to gain access to this memory page
      if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtec)) {
        // Loop through every byte to find the pattern
        SIZE_T maxOffset = mbi.RegionSize - count;
        for (SIZE_T i=0; i<maxOffset; i++) {
          LPVOID offset = (LPVOID)((UINT_PTR)(mbi.BaseAddress)+i);

          if (!memcmp(offset, search, count)) {
            // Found pattern, overwrite it
            memcpy(offset, replace, count);
            instances++;

            if (only_once) {
              break;
            }
          }
        }

        // Restore original permissions
        VirtualProtect(mbi.BaseAddress, mbi.RegionSize, oldProtec, &oldProtec);
      }
    }

    addr += mbi.RegionSize;
  }

  return instances;
}

LPVOID OverwriteImport(LPVOID imageBase, LPCSTR overrideFunction, LPVOID override)
{
  // Get DOS header
  PIMAGE_DOS_HEADER dosHeaders = (PIMAGE_DOS_HEADER)imageBase;

  // Retrieve NT header
  PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((UINT_PTR)imageBase + dosHeaders->e_lfanew);

  // Retrieve imports directory
  IMAGE_DATA_DIRECTORY importsDirectory = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

  // Retrieve import descriptor
  PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((UINT_PTR)imageBase + importsDirectory.VirtualAddress);

  while (importDescriptor->Name != NULL) {
    LPCSTR libraryName = (LPCSTR)importDescriptor->Name + (UINT_PTR)imageBase;
    HMODULE library = LoadLibraryA(libraryName);

    if (library) {
      PIMAGE_THUNK_DATA originalFirstThunk = (PIMAGE_THUNK_DATA)((UINT_PTR)imageBase + importDescriptor->OriginalFirstThunk);
      PIMAGE_THUNK_DATA firstThunk = (PIMAGE_THUNK_DATA)((UINT_PTR)imageBase + importDescriptor->FirstThunk);

      while (originalFirstThunk->u1.AddressOfData != NULL) {
        PIMAGE_IMPORT_BY_NAME functionName = (PIMAGE_IMPORT_BY_NAME)((UINT_PTR)imageBase + (UINT_PTR)originalFirstThunk->u1.AddressOfData);

        if (!strcmp((const char*)functionName->Name, overrideFunction)) {
          // Write to memory
          LPVOID originalFunction;
          WriteMemory(&firstThunk->u1.Function, &override, sizeof(LPVOID), &originalFunction);

          // Return original function and end loop here
          printf("Hooked %s\n", overrideFunction);
          return originalFunction;
        }
        ++originalFirstThunk;
        ++firstThunk;
      }
    }

    importDescriptor++;
  }

  return NULL;
}

LPVOID OverwriteVirtualTable(LPVOID object, SIZE_T methodIndex, LPVOID overrideFunction)
{
  LPVOID *vtable = ((LPVOID**)(object))[0];
  LPVOID &functionAddress = vtable[methodIndex];
  LPVOID originalFunction = NULL;

  WriteMemory(&functionAddress, overrideFunction ? &overrideFunction : NULL, sizeof(LPVOID), &originalFunction);

  return originalFunction;
}

LPVOID OverwriteCall(LPVOID destination, LPVOID localCall)
{
  char originalFunction[5];
  char callInst[5];

  callInst[0] = '\xE8';

  *(DWORD*)(&callInst[1]) = (DWORD)localCall - ((DWORD)destination + 5);

  if (!WriteMemory(destination, callInst, 5, originalFunction)) {
    return NULL;
  }

  return (LPVOID)((*(DWORD *)(originalFunction + 1)) + ((DWORD)destination + 5));
}

int OverwriteAllCalls(LPVOID imageBase, LPCVOID from, LPCVOID to)
{
  int count = 0;

  HANDLE process = GetCurrentProcess();
  MEMORY_BASIC_INFORMATION mbi = {0};

  DWORD fromAddr = (DWORD) from;
  DWORD toAddr = (DWORD) to;

  // Loop through memory pages
  UINT_PTR addr = (UINT_PTR)imageBase;
  while (VirtualQueryEx(process, (LPVOID)addr, &mbi, sizeof(mbi)) && mbi.AllocationBase == imageBase) {
    if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS) {
      DWORD oldProtec;

      // Try to gain access to this memory page
      if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtec)) {
        // Loop through every byte to find x86 calls
        SIZE_T maxOffset = mbi.RegionSize - count;
        for (SIZE_T i=0; i<maxOffset; i++) {
          unsigned char *offset = (unsigned char *)((UINT_PTR)(mbi.BaseAddress)+i);

          if ((*offset) == 0xE8) {
            // Found an x86 call instruction, see if it's to our function
            DWORD *func = (DWORD *) (offset + 1);
            DWORD adjustment = (DWORD) (offset + 5);

            if ((*func) == (fromAddr - adjustment)) {
              // Found a call to this function, replace it with our own
              *func = toAddr - adjustment;
              count++;
              i += 4;
            }
          }
        }

        // Restore original permissions
        VirtualProtect(mbi.BaseAddress, mbi.RegionSize, oldProtec, &oldProtec);
      }
    }

    addr += mbi.RegionSize;
  }

  return count;
}

LPVOID SearchPattern(LPVOID imageBase, LPCVOID search, SIZE_T count)
{
  LPVOID ret = NULL;

  HANDLE process = GetCurrentProcess();

  MEMORY_BASIC_INFORMATION mbi = {0};

  // Loop through memory pages
  UINT_PTR addr = (UINT_PTR)imageBase;
  while (VirtualQueryEx(process, (LPVOID)addr, &mbi, sizeof(mbi)) && mbi.AllocationBase == imageBase) {
    if (mbi.State == MEM_COMMIT && mbi.Protect != PAGE_NOACCESS) {
      DWORD oldProtec;

      // Try to gain access to this memory page
      if (VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &oldProtec)) {
        // Loop through every byte to find the pattern
        SIZE_T maxOffset = mbi.RegionSize - count;
        for (SIZE_T i=0; i<maxOffset; i++) {
          LPVOID offset = (LPVOID)((UINT_PTR)(mbi.BaseAddress)+i);

          if (!memcmp(offset, search, count)) {
            // Found pattern, overwrite it
            ret = offset;
            break;
          }
        }

        // Restore original permissions
        VirtualProtect(mbi.BaseAddress, mbi.RegionSize, oldProtec, &oldProtec);
      }
    }

    if (ret) {
      break;
    }

    addr += mbi.RegionSize;
  }

  return ret;
}

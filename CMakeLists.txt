cmake_minimum_required(VERSION 3.5)

project(Rebuilder LANGUAGES CXX)

set(CMAKE_MFC_FLAG 2)
add_compile_definitions(_AFXDLL)

#
# Build our code injected DLL
#
add_library(Rebld SHARED
  cmn/path.cpp
  cmn/path.h
  lib/config.cpp
  lib/config.h
  lib/dllmain.cpp
  lib/hooks.cpp
  lib/hooks.h
  lib/mmpassthru.cpp
  lib/util.cpp
  lib/util.h
  lib/worker.cpp
  lib/worker.h
)
target_compile_options(Rebld PRIVATE /MT)
target_link_libraries(Rebld PRIVATE winmm.lib shlwapi.lib)

# Add property grid
set(PROPERTYGRID_BUILD_APP OFF CACHE BOOL "")
add_subdirectory(ext/PropertyGrid)

#
# Build launcher/configuration executable
#
add_executable(Rebuilder WIN32
  cmn/path.cpp
  cmn/path.h
  res/res.rc
  res/resource.h
  src/app.cpp
  src/app.h
  src/clinkstatic.cpp
  src/clinkstatic.h
  src/launcher.cpp
  src/launcher.h
  src/patchgrid.cpp
  src/patchgrid.h
  src/window.cpp
  src/window.h
)

target_link_libraries(Rebuilder PRIVATE shlwapi.lib ddraw.lib dxguid.lib PropertyGrid)

# Ensure DLL is compiled before resource is built into executable
set_source_files_properties(res/res.rc PROPERTIES OBJECT_DEPENDS Rebld)

cmake_minimum_required(VERSION 3.5)

project(Rebuilder LANGUAGES CXX)

set(CMAKE_MFC_FLAG 2)
add_compile_definitions(_AFXDLL)

option(BUILD_UNICODE "Build with Unicode support" ON)

#
# Build our code injected DLL
#
add_library(Rebld SHARED
  lib/dllmain.cpp
  lib/hooks.cpp
  lib/hooks.h
  lib/mmpassthru.cpp
  lib/util.cpp
  lib/util.h
  lib/worker.cpp
  lib/worker.h
)
target_link_libraries(Rebld PRIVATE winmm.lib)

#
# Build launcher/configuration executable
#
add_executable(Rebuilder WIN32
  res/res.rc
  res/resource.h
  src/app.cpp
  src/app.h
  src/clinkstatic.cpp
  src/clinkstatic.h
  src/launcher.cpp
  src/launcher.h
  src/window.cpp
  src/window.h
)

if (BUILD_UNICODE)
  target_compile_definitions(Rebuilder PRIVATE UNICODE _UNICODE)
  target_compile_definitions(Rebld PRIVATE UNICODE _UNICODE)
  target_link_options(Rebuilder PRIVATE /entry:wWinMainCRTStartup)
endif()

target_link_libraries(Rebuilder PRIVATE shlwapi.lib)

# Ensure DLL is compiled before resource is built into executable
set_source_files_properties(res/res.rc PROPERTIES OBJECT_DEPENDS Rebld)

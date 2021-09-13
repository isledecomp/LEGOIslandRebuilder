cmake_minimum_required(VERSION 3.5)

project(Rebuilder LANGUAGES CXX)

set(CMAKE_MFC_FLAG 2)

option(BUILD_UNICODE "Build with Unicode support" ON)

add_executable(Rebuilder WIN32
  src/app.cpp
  src/app.h
  src/window.cpp
  src/window.h
)

target_compile_definitions(Rebuilder PRIVATE _AFXDLL)
if (BUILD_UNICODE)
  target_compile_definitions(Rebuilder PRIVATE UNICODE _UNICODE)
  target_link_options(Rebuilder PRIVATE /entry:wWinMainCRTStartup)
endif()

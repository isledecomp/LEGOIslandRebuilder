cmake_minimum_required(VERSION 3.5)

project(Rebuilder LANGUAGES CXX)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)

add_executable(Rebuilder WIN32
  src/app.cpp
  src/app.h
  src/window.cpp
  src/window.h
)

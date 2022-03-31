cmake_minimum_required(VERSION 3.5)

project(PropertyGrid LANGUAGES CXX)

set(CMAKE_MFC_FLAG 2)
add_compile_definitions(_AFXDLL)

option(PROPERTYGRID_BUILD_APP "Build with test application" ON)

add_library(PropertyGrid STATIC
  CustomItem.h
  DynDialogEx.cpp
  DynDialogEx.h
  DynDialogItemEx.cpp
  DynDialogItemEx.h
  ListDynDialogEx.cpp
  ListDynDialogEx.h
  PropertyGrid.cpp
  PropertyGrid.h
  PropertyGridCombo.cpp
  PropertyGridCombo.h
  PropertyGridDirectoryPicker.cpp
  PropertyGridDirectoryPicker.h
  PropertyGridInPlaceEdit.cpp
  PropertyGridInPlaceEdit.h
  PropertyGridMonthCalCtrl.cpp
  PropertyGridMonthCalCtrl.h
  stdafx.cpp
  stdafx.h
)

target_link_libraries(PropertyGrid PRIVATE shlwapi.lib)

if (PROPERTYGRID_BUILD_APP)
  add_subdirectory(test)
endif()

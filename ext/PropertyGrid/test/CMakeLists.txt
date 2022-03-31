add_executable(PropGrid WIN32
  CustomTreeCtrl.cpp
  CustomTreeCtrl.h
  GradientItem.cpp
  GradientItem.h
  PropGrid.cpp
  PropGrid.h
  PropGrid.rc
  PropGridDlg.cpp
  PropGridDlg.h
  RectEditDlg.cpp
  RectEditDlg.h
  RectItem.cpp
  RectItem.h
  resource.h
  SerialItem.cpp
  SerialItem.h
  TreeItem.cpp
  TreeItem.h
)

target_include_directories(PropGrid PRIVATE "..")
target_link_libraries(PropGrid PRIVATE PropertyGrid)

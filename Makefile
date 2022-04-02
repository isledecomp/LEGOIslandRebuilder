CC = cl
LD = link

DLL_SOURCES = \
  cmn/path.cpp \
  lib/config.cpp \
  lib/dllmain.cpp \
  lib/hooks.cpp \
  lib/mmpassthru.cpp \
  lib/util.cpp \

EXE_SOURCES = \
  cmn/path.cpp \
  ext/PropertyGrid/DynDialogEx.cpp \
  ext/PropertyGrid/DynDialogItemEx.cpp \
  ext/PropertyGrid/ListDynDialogEx.cpp \
  ext/PropertyGrid/PropertyGrid.cpp \
  ext/PropertyGrid/PropertyGridCombo.cpp \
  ext/PropertyGrid/PropertyGridDirectoryPicker.cpp \
  ext/PropertyGrid/PropertyGridInPlaceEdit.cpp \
  ext/PropertyGrid/PropertyGridMonthCalCtrl.cpp \
  ext/PropertyGrid/stdafx.cpp \
  src/app.cpp \
  src/clinkstatic.cpp \
  src/launcher.cpp \
  src/patchgrid.cpp \
  src/tabs.cpp \
  src/window.cpp \

DLL_OBJECTS = $(DLL_SOURCES:.cpp=.obj)
EXE_OBJECTS = $(EXE_SOURCES:.cpp=.obj)

EXE = Rebuilder.exe
DLL = Rebld.dll

RES = res/res.res
RES_SRC = res/res.rc

$(EXE) : $(EXE_OBJECTS) $(RES)
  $(LD) /out:$@ /subsystem:windows $** shlwapi.lib ddraw.lib dxguid.lib

.cpp.obj :
  $(CC) /nologo /Fo$@ -D_AFXDLL -MD -c $<

$(DLL) : $(DLL_OBJECTS)
  $(LD) /out:$@ /dll $** user32.lib winmm.lib shlwapi.lib advapi32.lib

$(RES) : $(RES_SRC) $(DLL)

clean :
  del /s /q *.obj *.res *.dll *.exe *.exp *.lib *.res
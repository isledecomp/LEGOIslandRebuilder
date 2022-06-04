# Microsoft Developer Studio Project File - Name="Rebuilder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=REBUILDER - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Rebuilder.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Rebuilder.mak" CFG="REBUILDER - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Rebuilder - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 shlwapi.lib ddraw.lib dxguid.lib /nologo /subsystem:windows /machine:I386
# Begin Target

# Name "Rebuilder - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\app.cpp
# End Source File
# Begin Source File

SOURCE=..\src\clinkstatic.cpp
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\DynDialogEx.cpp
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\DynDialogItemEx.cpp
# End Source File
# Begin Source File

SOURCE=..\src\launcher.cpp
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\ListDynDialogEx.cpp
# End Source File
# Begin Source File

SOURCE=..\src\patchgrid.cpp
# End Source File
# Begin Source File

SOURCE=..\cmn\path.cpp
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\PropertyGrid.cpp
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\PropertyGridCombo.cpp
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\PropertyGridDirectoryPicker.cpp
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\PropertyGridInPlaceEdit.cpp
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\PropertyGridMonthCalCtrl.cpp
# End Source File
# Begin Source File

SOURCE=..\res\res.rc
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=..\src\tabs.cpp
# End Source File
# Begin Source File

SOURCE=..\src\window.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\app.h
# End Source File
# Begin Source File

SOURCE=..\src\clinkstatic.h
# End Source File
# Begin Source File

SOURCE=..\cmn\combo.h
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\CustomItem.h
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\DynDialogEx.h
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\DynDialogItemEx.h
# End Source File
# Begin Source File

SOURCE=..\src\launcher.h
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\ListDynDialogEx.h
# End Source File
# Begin Source File

SOURCE=..\src\patchgrid.h
# End Source File
# Begin Source File

SOURCE=..\cmn\path.h
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\PropertyGrid.h
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\PropertyGridCombo.h
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\PropertyGridDirectoryPicker.h
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\PropertyGridInPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\PropertyGridMonthCalCtrl.h
# End Source File
# Begin Source File

SOURCE=..\res\resource.h
# End Source File
# Begin Source File

SOURCE=..\ext\PropertyGrid\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\src\tabs.h
# End Source File
# Begin Source File

SOURCE=..\src\window.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\res\mama.ico
# End Source File
# End Group
# Begin Source File

SOURCE=..\res\res.manifest
# End Source File
# End Target
# End Project

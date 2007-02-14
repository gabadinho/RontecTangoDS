# Microsoft Developer Studio Project File - Name="Rontec" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=Rontec - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Rontec.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Rontec.mak" CFG="Rontec - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Rontec - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "Rontec - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "Rontec - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f Rontec.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Rontec.exe"
# PROP BASE Bsc_Name "Rontec.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f Makefile.VC"
# PROP Rebuild_Opt "/a"
# PROP Target_File "c:\DeviceServers\ds_Rontec.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "Rontec - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f Rontec.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "Rontec.exe"
# PROP BASE Bsc_Name "Rontec.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "nmake /f Makefile.VC"
# PROP Rebuild_Opt "clean all"
# PROP Target_File "c:\DeviceServers\ds_Rontec.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "Rontec - Win32 Release"
# Name "Rontec - Win32 Debug"

!IF  "$(CFG)" == "Rontec - Win32 Release"

!ELSEIF  "$(CFG)" == "Rontec - Win32 Debug"

!ENDIF 

# Begin Group "src"

# PROP Default_Filter "*.cpp;*.h"
# Begin Source File

SOURCE=..\src\ClassFactory.cpp
# End Source File
# Begin Source File

SOURCE=..\src\main.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Rontec.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Rontec.h
# End Source File
# Begin Source File

SOURCE=..\src\RontecClass.cpp
# End Source File
# Begin Source File

SOURCE=..\src\RontecClass.h
# End Source File
# Begin Source File

SOURCE=..\src\RontecImpl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\RontecImpl.h
# End Source File
# Begin Source File

SOURCE=..\src\RontecStateMachine.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Makefile.vc
# End Source File
# End Target
# End Project

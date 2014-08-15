# Microsoft Developer Studio Project File - Name="re" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=re - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "re.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "re.mak" CFG="re - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "re - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "re - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "re - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "UNICODE" /YX"./inc/_Common.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "re - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RE_EXPORTS" /YX /FD /GZ  /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "inc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "UNICODE" /D "DEBUG" /YX"./inc/_Common.h" /FD /GZ  /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "re - Win32 Release"
# Name "re - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\antievt.cpp
# End Source File
# Begin Source File

SOURCE=.\src\array.cpp
# End Source File
# Begin Source File

SOURCE=.\src\callmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cfpf.cpp
# End Source File
# Begin Source File

SOURCE=.\src\clasifyc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\coleobj.cpp
# End Source File
# Begin Source File

SOURCE=.\src\devdsc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dfreeze.cpp
# End Source File
# Begin Source File

SOURCE=.\src\disp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dispml.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dispprt.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dispsl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\doc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dragdrp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dxfrobj.cpp
# End Source File
# Begin Source File

SOURCE=.\src\edit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\font.cpp
# End Source File
# Begin Source File

SOURCE=.\src\format.cpp
# End Source File
# Begin Source File

SOURCE=.\src\frunptr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\hash.cpp
# End Source File
# Begin Source File

SOURCE=.\src\host.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ime.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ldte.cpp
# End Source File
# Begin Source File

SOURCE=.\src\line.cpp
# End Source File
# Begin Source File

SOURCE=.\src\m_undo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\macddrop.cxx
# End Source File
# Begin Source File

SOURCE=.\src\magellan.cpp
# End Source File
# Begin Source File

SOURCE=.\src\measure.cpp
# End Source File
# Begin Source File

SOURCE=.\src\nlsprocs.cpp
# End Source File
# Begin Source File

SOURCE=.\src\notmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\object.cpp
# End Source File
# Begin Source File

SOURCE=.\src\objmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\osdc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pensup.cpp
# End Source File
# Begin Source File

SOURCE=.\src\propchg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\range.cpp
# End Source File
# Begin Source File

SOURCE=.\src\reinit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\remain.cpp
# End Source File
# Begin Source File

SOURCE=.\src\render.cpp
# End Source File
# Begin Source File

SOURCE=.\src\richedit.rc
# End Source File
# Begin Source File

SOURCE=.\src\rtext.cpp
# End Source File
# Begin Source File

SOURCE=.\src\rtflex.cpp
# End Source File
# Begin Source File

SOURCE=.\src\rtflog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\rtfread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\rtfread2.cpp
# End Source File
# Begin Source File

SOURCE=.\src\rtfwrit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\rtfwrit2.cpp
# End Source File
# Begin Source File

SOURCE=.\src\runptr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\select.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sift.cpp
# End Source File
# Begin Source File

SOURCE=.\src\text.cpp
# End Source File
# Begin Source File

SOURCE=.\src\textserv.cpp
# End Source File
# Begin Source File

SOURCE=.\src\tokens.cpp
# End Source File
# Begin Source File

SOURCE=.\src\tomdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\tomfmt.cpp
# End Source File
# Begin Source File

SOURCE=.\src\tomrange.cpp
# End Source File
# Begin Source File

SOURCE=.\src\tomsel.cpp
# End Source File
# Begin Source File

SOURCE=.\src\unicwrap.cpp
# End Source File
# Begin Source File

SOURCE=.\src\urlsup.cpp
# End Source File
# Begin Source File

SOURCE=.\src\util.cpp
# End Source File
# Begin Source File

SOURCE=.\src\uuid.cpp
# End Source File
# Begin Source File

SOURCE=.\src\win2mac.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\_antievt.h
# End Source File
# Begin Source File

SOURCE=.\src\_array.h
# End Source File
# Begin Source File

SOURCE=.\src\_callmgr.h
# End Source File
# Begin Source File

SOURCE=.\src\_cfpf.h
# End Source File
# Begin Source File

SOURCE=.\src\_clasfyc.h
# End Source File
# Begin Source File

SOURCE=.\src\_coleobj.h
# End Source File
# Begin Source File

SOURCE=.\src\_common.h
# End Source File
# Begin Source File

SOURCE=.\src\_devdsc.h
# End Source File
# Begin Source File

SOURCE=.\src\_devstk.h
# End Source File
# Begin Source File

SOURCE=.\src\_devstki.h
# End Source File
# Begin Source File

SOURCE=.\src\_dfreeze.h
# End Source File
# Begin Source File

SOURCE=.\src\_disp.h
# End Source File
# Begin Source File

SOURCE=.\src\_dispml.h
# End Source File
# Begin Source File

SOURCE=.\src\_dispprt.h
# End Source File
# Begin Source File

SOURCE=.\src\_dispsl.h
# End Source File
# Begin Source File

SOURCE=.\src\_doc.h
# End Source File
# Begin Source File

SOURCE=.\src\_dragdrp.h
# End Source File
# Begin Source File

SOURCE=.\src\_drwinfo.h
# End Source File
# Begin Source File

SOURCE=.\src\_dte.h
# End Source File
# Begin Source File

SOURCE=.\src\_dxfrobj.h
# End Source File
# Begin Source File

SOURCE=.\src\_edit.h
# End Source File
# Begin Source File

SOURCE=.\inc\_exchext.h
# End Source File
# Begin Source File

SOURCE=.\src\_font.h
# End Source File
# Begin Source File

SOURCE=.\src\_format.h
# End Source File
# Begin Source File

SOURCE=.\src\_frunptr.h
# End Source File
# Begin Source File

SOURCE=.\src\_host.h
# End Source File
# Begin Source File

SOURCE=.\src\_ime.h
# End Source File
# Begin Source File

SOURCE=.\src\_invar.h
# End Source File
# Begin Source File

SOURCE=.\src\_ldte.h
# End Source File
# Begin Source File

SOURCE=.\src\_line.h
# End Source File
# Begin Source File

SOURCE=.\src\_m_undo.h
# End Source File
# Begin Source File

SOURCE=.\src\_magelln.h
# End Source File
# Begin Source File

SOURCE=.\inc\_mailext.h
# End Source File
# Begin Source File

SOURCE=.\src\_measure.h
# End Source File
# Begin Source File

SOURCE=.\src\_nlsprcs.h
# End Source File
# Begin Source File

SOURCE=.\src\_notmgr.h
# End Source File
# Begin Source File

SOURCE=.\src\_objmgr.h
# End Source File
# Begin Source File

SOURCE=.\src\_osdc.h
# End Source File
# Begin Source File

SOURCE=.\src\_pensup.h
# End Source File
# Begin Source File

SOURCE=.\src\_range.h
# End Source File
# Begin Source File

SOURCE=.\src\_render.h
# End Source File
# Begin Source File

SOURCE=.\src\_rtext.h
# End Source File
# Begin Source File

SOURCE=.\src\_rtfconv.h
# End Source File
# Begin Source File

SOURCE=.\src\_rtflog.h
# End Source File
# Begin Source File

SOURCE=.\src\_rtfread.h
# End Source File
# Begin Source File

SOURCE=.\src\_rtfwrit.h
# End Source File
# Begin Source File

SOURCE=.\src\_runptr.h
# End Source File
# Begin Source File

SOURCE=.\src\_select.h
# End Source File
# Begin Source File

SOURCE=.\src\_sift.h
# End Source File
# Begin Source File

SOURCE=.\src\_text.h
# End Source File
# Begin Source File

SOURCE=.\src\_tomdoc.h
# End Source File
# Begin Source File

SOURCE=.\src\_tomfmt.h
# End Source File
# Begin Source File

SOURCE=.\src\_undoid.h
# End Source File
# Begin Source File

SOURCE=.\src\_urlsup.h
# End Source File
# Begin Source File

SOURCE=.\src\_util.h
# End Source File
# Begin Source File

SOURCE=.\src\_uwrap.h
# End Source File
# Begin Source File

SOURCE=.\inc\_wmsui.h
# End Source File
# Begin Source File

SOURCE=.\inc\addrbook.h
# End Source File
# Begin Source File

SOURCE=.\inc\capext.h
# End Source File
# Begin Source File

SOURCE=.\inc\capguid.h
# End Source File
# Begin Source File

SOURCE=.\inc\caphelp.h
# End Source File
# Begin Source File

SOURCE=.\inc\version\capone.h
# End Source File
# Begin Source File

SOURCE=.\inc\chico.h
# End Source File
# Begin Source File

SOURCE=.\inc\comcrit.h
# End Source File
# Begin Source File

SOURCE=.\inc\convert.h
# End Source File
# Begin Source File

SOURCE=.\inc\csd.h
# End Source File
# Begin Source File

SOURCE=.\inc\dbug32.h
# End Source File
# Begin Source File

SOURCE=.\inc\dbugit.h
# End Source File
# Begin Source File

SOURCE=.\src\drawdata.h
# End Source File
# Begin Source File

SOURCE=.\inc\exchext.h
# End Source File
# Begin Source File

SOURCE=.\inc\freedoc.h
# End Source File
# Begin Source File

SOURCE=.\inc\global.h
# End Source File
# Begin Source File

SOURCE=.\src\hash.h
# End Source File
# Begin Source File

SOURCE=.\inc\imeshare.h
# End Source File
# Begin Source File

SOURCE=.\inc\library.h
# End Source File
# Begin Source File

SOURCE=.\inc\lmsmapi.h
# End Source File
# Begin Source File

SOURCE=.\inc\mailext.h
# End Source File
# Begin Source File

SOURCE=.\inc\mailexts.h
# End Source File
# Begin Source File

SOURCE=.\inc\mailview.h
# End Source File
# Begin Source File

SOURCE=.\inc\mlcfg.h
# End Source File
# Begin Source File

SOURCE=.\inc\mlfind.h
# End Source File
# Begin Source File

SOURCE=.\inc\mlrmui.h
# End Source File
# Begin Source File

SOURCE=.\inc\mni.h
# End Source File
# Begin Source File

SOURCE=.\inc\msmail.h
# End Source File
# Begin Source File

SOURCE=.\inc\ole2ui.h
# End Source File
# Begin Source File

SOURCE=.\inc\olenls.h
# End Source File
# Begin Source File

SOURCE=.\inc\olestd.h
# End Source File
# Begin Source File

SOURCE=.\inc\ourmac.h
# End Source File
# Begin Source File

SOURCE=.\inc\ourtypes.h
# End Source File
# Begin Source File

SOURCE=.\inc\preinc.h
# End Source File
# Begin Source File

SOURCE=.\inc\profiles.h
# End Source File
# Begin Source File

SOURCE=.\inc\prsht.h
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=.\inc\richedit.h
# End Source File
# Begin Source File

SOURCE=.\inc\sc.h
# End Source File
# Begin Source File

SOURCE=.\inc\statbar.h
# End Source File
# Begin Source File

SOURCE=.\inc\tchar16.h
# End Source File
# Begin Source File

SOURCE=.\src\textserv.h
# End Source File
# Begin Source File

SOURCE=.\src\tokens.h
# End Source File
# Begin Source File

SOURCE=.\inc\tom.h
# End Source File
# Begin Source File

SOURCE=.\inc\unitable.h
# End Source File
# Begin Source File

SOURCE=.\inc\unknwn.h
# End Source File
# Begin Source File

SOURCE=.\inc\version\version.h
# End Source File
# Begin Source File

SOURCE=.\inc\vlb.h
# End Source File
# Begin Source File

SOURCE=.\inc\win2mac.h
# End Source File
# Begin Source File

SOURCE=.\inc\winwlm.h
# End Source File
# Begin Source File

SOURCE=.\inc\wlmimm.h
# End Source File
# Begin Source File

SOURCE=.\inc\wmsui.h
# End Source File
# Begin Source File

SOURCE=.\inc\wmsuiext.h
# End Source File
# Begin Source File

SOURCE=.\src\wrapdefs.h
# End Source File
# Begin Source File

SOURCE=.\src\wrapfns.h
# End Source File
# Begin Source File

SOURCE=.\inc\zmouse.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\src\dragcopy.cur
# End Source File
# Begin Source File

SOURCE=.\src\dragmove.cur
# End Source File
# Begin Source File

SOURCE=.\src\dragnone.cur
# End Source File
# Begin Source File

SOURCE=.\src\east.cur
# End Source File
# Begin Source File

SOURCE=.\src\ne.cur
# End Source File
# Begin Source File

SOURCE=.\src\nomove2d.cur
# End Source File
# Begin Source File

SOURCE=.\src\nomoveh.cur
# End Source File
# Begin Source File

SOURCE=.\src\nomovev.cur
# End Source File
# Begin Source File

SOURCE=.\src\north.cur
# End Source File
# Begin Source File

SOURCE=.\src\nw.cur
# End Source File
# Begin Source File

SOURCE=.\src\se.cur
# End Source File
# Begin Source File

SOURCE=.\src\selbar.cur
# End Source File
# Begin Source File

SOURCE=.\src\selbarm.cur
# End Source File
# Begin Source File

SOURCE=.\src\south.cur
# End Source File
# Begin Source File

SOURCE=.\src\sw.cur
# End Source File
# Begin Source File

SOURCE=.\src\west.cur
# End Source File
# End Group
# End Target
# End Project

; NSIS script to create an ASCEND binary installer for Windows
; by John Pye, 2006-2012.
;
;--------------------------------

; The name of the installer

Name "ASCEND ${VERSION}"

;SetCompressor /FINAL zlib
SetCompressor /SOLID lzma

!include LogicLib.nsh
!include nsDialogs.nsh
!include x64.nsh

; The file to write
OutFile ${OUTFILE}

!if "${INSTARCH}" == "x64"
!define INST64
!endif

; The default installation directory
!ifdef INST64
InstallDir $PROGRAMFILES64\ASCEND
!define SMNAME "ASCEND (64-bit)"
!else
InstallDir $PROGRAMFILES32\ASCEND
!define SMNAME "ASCEND"
!endif

; NOTE we *don't* user InstallDirRegKey because it doesn't work correctly on Win64.
;InstallDirRegKey HKLM "Software\ASCEND" "Install_Dir"

RequestExecutionLevel admin

;--------------------------------

; Pages

Page license
LicenseData "..\LICENSE.txt"

Page components
Page directory
Page custom dependenciesCreate dependenciesLeave
Page instfiles
Page custom ascendIniCreate ascendIniLeave
Page custom ascendEnvVarCreate ascendEnvVarLeave

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

!define GTKSEARCHPATH "c:\GTK"

Var DEFAULTPATH
Var HAVE_PYTHON
Var PYPATH
Var HAVE_GTK
Var HAVE_GTKSOURCEVIEW
Var HAVE_GAPHAS
Var HAVE_SIMPLEGENERIC
Var HAVE_DECORATOR
Var PYINSTALLED

Var PDFINSTALLED

Var PATH
Var GTK_GUI

Var NEED_PYTHON
Var NEED_GTK
Var NEED_GTKSOURCEVIEW
Var NEED_GAPHAS
Var NEED_SIMPLEGENERIC
Var NEED_DECORATOR

Var ASCENDINIFOUND
Var ASCENDENVVARFOUND
Var ASCENDLIBRARY

Var PYTHONTARGETDIR

; .onInit has been moved to after section decls so that they can be references

;------------------------------------------------------------
; DOWNLOAD AND INSTALL DEPENDENCIES FIRST

; Use the official python.org Python packages
!define PYTHON_VERSION "${PYVERSION}${PYPATCH}"
!define PYTHON_FN "python-${PYTHON_VERSION}${PYARCH}.msi"
!define PYTHON_URL "http://python.org/ftp/python/${PYTHON_VERSION}/${PYTHON_FN}"
!define PYTHON_CMD "msiexec /i $DAI_TMPFILE /passive ALLUSERS=1 TARGETDIR=$PYTHONTARGETDIR"

!define THIRDPARTY_DIR "http://downloads.sourceforge.net/project/ascend-sim/thirdparty/"
!define TMP_DROPBOX_DIR "https://dl.dropboxusercontent.com/u/79623370/ascend/better/"

!ifdef INST64
!define WINXX "win64"
!define AMDXX ".win-amd64"
!define NNBIT "64-bit"
!define X64I386 "x64"
!else
!define WINXX "win32"
!define AMDXX ".win32"
!define X64I386 "i386"
!define NNBIT "32-bit"
!endif

!define PYGI_VER "3.14.0"
!define PYGI_REV "rev18"
!define PYGI_FN "pygi-aio-${PYGI_VER}_${PYGI_REV}-min-setup.exe"
!define PYGI_URL "${TMP_DROPBOX_DIR}${PYGI_FN}"
!define PYGI_CMD "$DAI_TMPFILE"

!define SG_VER "0.8.1"
!define GAP_VER "0.8.0"
!define DEC_VER "3.4.2"
!define SG_FN "simplegeneric-${SG_VER}${AMDXX}-py${PYVERSION}.exe"
!define DEC_FN "decorator-${DEC_VER}${AMDXX}-py${PYVERSION}.exe"
!define GAP_FN "gaphas-${GAP_VER}${AMDXX}-py${PYVERSION}.exe"
!define SG_URL "${TMP_DROPBOX_DIR}${SG_FN}"
!define DEC_URL "${TMP_DROPBOX_DIR}${DEC_FN}"
!define GAP_URL "${TMP_DROPBOX_DIR}${GAP_FN}"
!define SG_CMD "$DAI_TMPFILE"
!define DEC_CMD "$DAI_TMPFILE"
!define GAP_CMD "$DAI_TMPFILE"

!include "download.nsi"

Section "-python"
	DetailPrint "--- DOWNLOAD PYTHON ---"
        ${If} $NEED_PYTHON == '1'
		!insertmacro downloadAndInstall "Python" "${PYTHON_URL}" "${PYTHON_FN}" "${PYTHON_CMD}"
		Call DetectPython
		${If} $HAVE_PYTHON == 'NOK'
			MessageBox MB_OK "Python installation appears to have failed. You may need to retry manually."
		${EndIf}
        ${EndIf}
SectionEnd

Section "-pygi"
	DetailPrint "--- DOWNLOAD PYGI ---"
        ${If} $NEED_GTK == '1'
		${OrIf} $NEED_GTKSOURCEVIEW == '1'
		!insertmacro downloadAndInstall "PyGI" "${PYGI_URL}" "${PYGI_FN}" "${PYGI_CMD}"
		Call DetectGTK
		Call DetectGTKSourceView
		${If} $HAVE_GTK == 'NOK'
			MessageBox MB_OK "GTK installation appears to have failed. You may need to retry manually."
		${EndIf}
		${If} $HAVE_GTKSOURCEVIEW == 'NOK'
			MessageBox MB_OK "GTKSourceView installation appears to have failed. You may need to retry manually."
		${EndIf}
        ${EndIf}
SectionEnd

Section "-simplegeneric"
	DetailPrint "--- DOWNLOAD SIMPLEGENERIC ---"
        ${If} $NEED_SIMPLEGENERIC == '1'
		!insertmacro downloadAndInstall "SimpleGeneric" "${SG_URL}" "${SG_FN}" "${SG_CMD}"
		Call DetectSimpleGeneric
		${If} $HAVE_SIMPLEGENERIC == 'NOK'
			MessageBox MB_OK "SimpleGeneric installation appears to have failed. You may need to retry manually."
		${EndIf}
        ${EndIf}
SectionEnd

Section "-decorator"
	DetailPrint "--- DOWNLOAD DECORATOR ---"
        ${If} $NEED_DECORATOR == '1'
		!insertmacro downloadAndInstall "Decorator" "${DEC_URL}" "${DEC_FN}" "${DEC_CMD}"
		Call DetectDecorator
		${If} $HAVE_DECORATOR == 'NOK'
			MessageBox MB_OK "Decorator installation appears to have failed. You may need to retry manually."
		${EndIf}
        ${EndIf}
SectionEnd

Section "-gaphas"
	DetailPrint "--- DOWNLOAD GAPHAS ---"
        ${If} $NEED_GAPHAS == '1'
		!insertmacro downloadAndInstall "Gaphas" "${GAP_URL}" "${GAP_FN}" "${GAP_CMD}"
		Call DetectGaphas
		${If} $HAVE_GAPHAS == 'NOK'
			MessageBox MB_OK "Gaphas installation appears to have failed. You may need to retry manually."
		${EndIf}
        ${EndIf}
SectionEnd
;------------------------------------------------------------------------
; INSTALL CORE STUFF including model library

; The stuff to install
Section "ASCEND (required)"
	SectionIn RO

	DetailPrint "--- COMMON FILES ---"

	; Set output path to the installation directory.
	SetOutPath $INSTDIR
	File "..\ascend.dll"
	File "..\ascend-config"
	File "..\pygtk\glade\ascend.ico"
	File "..\LICENSE.txt"
	File "..\CHANGELOG.txt"
	File "..\README-windows.txt"
	${FILE_IPOPT_1}
	${FILE_IPOPT_2}
	${FILE_IPOPT_3}
	${FILE_IPOPT_4}
	${FILE_IPOPT_5}
	
	; Model Library
	SetOutPath $INSTDIR\models
	File /r /x .svn "..\models\*.a4*"
	File /r /x .svn "..\models\*.tcl"
	File /r /x .svn "..\models\*_ascend.dll" ; extension modules
	File /r /x .svn "..\models\*.py"; python modules
	
	SetOutPath $INSTDIR\solvers
	File "..\solvers\qrslv\qrslv_ascend.dll"
	File "..\solvers\conopt\conopt_ascend.dll"
	File "..\solvers\lrslv\lrslv_ascend.dll"
	File "..\solvers\cmslv\cmslv_ascend.dll"
	File "..\solvers\lsode\lsode_ascend.dll"
	File "..\solvers\ida\ida_ascend.dll"
	File "..\solvers\dopri5\dopri5_ascend.dll"
	File "..\solvers\ipopt\ipopt_ascend.dll"
	
	SetOutPath $INSTDIR
	;File "Makefile.bt"
	File "..\tools\textpad\ascend.syn"

	; Check for pre-existing .ascend.ini for current user (warn after installation, if so)
	${If} ${FileExists} "$APPDATA\.ascend.ini"
		StrCpy $ASCENDINIFOUND "1"
	${Else}
		; Set 'librarypath' in .ascend.ini
		WriteINIstr $APPDATA\.ascend.ini Directories librarypath "$DOCUMENTS\ascdata;$INSTDIR\models"
	${EndIf}
	
	; Check for ASCENDLIBRARY environment variable for current user
	ExpandEnvStrings $ASCENDLIBRARY "%ASCENDLIBRARY%"
	${IfNot} $ASCENDLIBRARY == "%ASCENDLIBRARY%"
		StrCpy $ASCENDENVVARFOUND "1"
	${EndIf}
	
	; Write the installation path into the registry
	WriteRegStr HKLM SOFTWARE\ASCEND "Install_Dir" "$INSTDIR"

	; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ASCEND" "DisplayName" "ASCEND Simulation Environment"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ASCEND" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ASCEND" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ASCEND" "NoRepair" 1
	WriteUninstaller "uninstall.exe"

	; Write file locations to the registry for access from ascend-config
	WriteRegStr HKLM SOFTWARE\ASCEND "INSTALL_LIB" "$INSTDIR"
	WriteRegStr HKLM SOFTWARE\ASCEND "INSTALL_BIN" "$INSTDIR"
	WriteRegStr HKLM SOFTWARE\ASCEND "INSTALL_INCLUDE" "$INSTDIR\include"
	WriteRegStr HKLM SOFTWARE\ASCEND "INSTALL_ASCDATA" "$INSTDIR"
	WriteRegStr HKLM SOFTWARE\ASCEND "INSTALL_MODELS" "$INSTDIR\models"
	WriteRegStr HKLM SOFTWARE\ASCEND "INSTALL_SOLVERS" "$INSTDIR\solvers"
	
	; Write default values of ASCENDLIBRARY and ASCENDSOLVERS (user can override with env vars)
	WriteRegStr HKLM SOFTWARE\ASCEND "ASCENDLIBRARY" "$INSTDIR\models"
	WriteRegStr HKLM SOFTWARE\ASCEND "ASCENDSOLVERS" "$INSTDIR\solvers"
		
	Return
SectionEnd

;--------------------------------
Section "GTK GUI" sect_pygtk
!ifdef INST64
	SetRegView 64
!endif
	StrCpy $GTK_GUI "YES"
	; Check the dependencies of the GTK GUI before proceding...
	${If} $HAVE_PYTHON == 'NOK'
		MessageBox MB_OK "GTK GUI can not be installed, because Python was not found on this system.$\nIf you do want to use the GTK GUI, please check the installation instructions$\n$\n(PYPATH=$PYPATH)"
	${ElseIf} $HAVE_GTK == 'NOK'
		MessageBox MB_OK "GTK GUI cannot be installed, because GTK+ 3.x was not found on this system.$\nIf you do want to use the GTK GUI, please install GTK3 from PyGObject for Windows$\n"
	${ElseIf} $HAVE_GTKSOURCEVIEW == 'NOK'
		MessageBox MB_OK "GTK GUI cannot be installed, because GTKSourceView was not found on this system.$\nIf you do want to use the GTK GUI, please install GTKSourceView from PyGObject for Windows$\n"
	${Else}
		;MessageBox MB_OK "Python: $PYPATH, GTK: $GTKPATH"

		DetailPrint "--- PYTHON INTERFACE ---"

		; File icon
		SetOutPath $INSTDIR
		File "..\pygtk\glade\ascend-doc.ico"
		File "..\pygtk\ascend"		

		; Python interface
		SetOutPath $INSTDIR\python
		File "..\ascxx\_ascpy.pyd"
		File "..\ascxx\*.py"
		File "..\pygtk\*.py"
		
		; FPROPS: python bindings
		File "..\models\johnpye\fprops\python\_fprops.pyd"
		File "..\models\johnpye\fprops\python\*.py"
		
		; GLADE assets
		SetOutPath $INSTDIR\glade
		File "..\pygtk\glade\*.glade"
		File "..\pygtk\glade\*.png"
		File "..\pygtk\glade\*.svg"

		StrCpy $PYINSTALLED "1"
		WriteRegDWORD HKLM "SOFTWARE\ASCEND" "Python" 1	

		; Create 'ascend.bat' batch file for launching the PyGTK GUI.
		ClearErrors
		FileOpen $0 $INSTDIR\ascend.bat w
		${If} ${Errors}
			MessageBox MB_OK "The 'ascend.bat' file was not installed properly; problems writing to that file."	
		${Else}
			FileWrite $0 "@echo off"
			FileWriteByte $0 "13"
			FileWriteByte $0 "10"
			FileWrite $0 "set PATH=$PATH"
			FileWriteByte $0 "13"
			FileWriteByte $0 "10"
			FileWrite $0 "cd "
			FileWrite $0 $INSTDIR 
			FileWriteByte $0 "13"
			FileWriteByte $0 "10"
			FileWrite $0 "$PYPATH\python "
			FileWriteByte $0 "34" 
			FileWrite $0 "$INSTDIR\ascend"
			FileWriteByte $0 "34"
			FileWrite $0 " %1 %2 %3 %4 %5 %6 %7 %8"
			FileWriteByte $0 "13"
			FileWriteByte $0 "10"
			FileClose $0
		${EndIf}

		;---- file association ----

		; back up old value of .a4c file association
		ReadRegStr $1 HKCR ".a4c" ""
		StrCmp $1 "" a4cnobkp
		StrCmp $1 "ASCEND.model" a4cnobkp

		; Remember the old file association if necessary
		WriteRegStr HKLM "SOFTWARE\ASCEND" "BackupAssocA4C" $1

a4cnobkp:
		WriteRegStr HKCR ".a4c" "" "ASCEND.model"

		; back up old value of .a4c file association
		ReadRegStr $1 HKCR ".a4l" ""
		StrCmp $1 "" a4lnobkp
		StrCmp $1 "ASCEND.model" a4lnobkp

		; Remember the old file association if necessary
		WriteRegStr HKLM "SOFTWARE\ASCEND" "BackupAssocA4L" $1

a4lnobkp:
		WriteRegStr HKCR ".a4l" "" "ASCEND.model"

		; So, what does an A4L or A4C file actually do?

		ReadRegStr $0 HKCR "ASCEND.model" ""
		StrCmp $0 "" 0 a4cskip

		WriteRegStr HKCR "ASCEND.model" "" "ASCEND model file"
		WriteRegStr HKCR "ASCEND.model\shell" "" "open"
		WriteRegStr HKCR "ASCEND.model\DefaultIcon" "" "$INSTDIR\ascend-doc.ico"

a4cskip:
		WriteRegStr HKCR "ASCEND.model\shell\open\command" "" '$PYPATH\pythonw.exe "$INSTDIR\ascend" "%1"'

		System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'

	${EndIf}
	Return

SectionEnd

Section "Canvas GUI" sect_canvas
!ifdef INST64
	SetRegView 64
!endif
	; Check the dependencies of the Canvas GUI before proceding...
	${If} $GTK_GUI == 'NO'
		MessageBox MB_OK "Canvas cannot be installed, because GTK GUI was not installed on this system.$\nIf you do want to use the Canvas, please firstly install GTK GUI$\n"
	${ElseIf} $HAVE_PYTHON == 'NOK'
		MessageBox MB_OK "Canvas cannot be installed, because Python was not found on this system.$\nIf you do want to use the Canvas, please check the installation instructions$\n$\n(PYPATH=$PYPATH)"
	${ElseIf} $HAVE_GTK == 'NOK'
		MessageBox MB_OK "Canvas cannot be installed, because GTK+ 3.x was not found on this system.$\nIf you do want to use the Canvas, please install GTK3 from PyGObject for Windows$\n"
	${ElseIf} $HAVE_GTKSOURCEVIEW == 'NOK'
		MessageBox MB_OK "Canvas cannot be installed, because GTKSourceView was not found on this system.$\nIf you do want to use the Canvas, please install GTKSourceView from PyGObject for Windows$\n"
	${ElseIf} $HAVE_SIMPLEGENERIC == 'NOK'
		MessageBox MB_OK "Canvas cannot be installed, because SimpleGeneric was not found on this system.$\nIf you do want to use the Canvas, please install SimpleGeneric python package$\n"
	${ElseIf} $HAVE_DECORATOR == 'NOK'
		MessageBox MB_OK "Canvas cannot be installed, because Decorator was not found on this system.$\nIf you do want to use the Canvas, please install Decorator python package$\n"
	${ElseIf} $HAVE_GAPHAS == 'NOK'
		MessageBox MB_OK "Canvas cannot be installed, because Gaphas was not found on this system.$\nIf you do want to use the Canvas, please install Gaphas python package$\n"
	${Else}
		;MessageBox MB_OK "Python: $PYPATH, GTK: $GTKPATH"

		DetailPrint "--- Canvas ---"
		; Python interface
		SetOutPath $INSTDIR\python
		File "..\pygtk\canvas\*.py"

		WriteRegDWORD HKLM "SOFTWARE\ASCEND" "Canvas" 1	
	${EndIf}
	Return

SectionEnd

;---------------------------------

Section "Documentation" sect_doc
	SetOutPath $INSTDIR
	File "..\doc\book.pdf"
	StrCpy $PDFINSTALLED "1"
	WriteRegDWORD HKLM "SOFTWARE\ASCEND" "PDF" 1
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts" sect_menu
  
	WriteRegDWORD HKLM "SOFTWARE\ASCEND" "StartMenu" 1

	CreateDirectory "$SMPROGRAMS\${SMNAME}"  

	; Link to PyGTK GUI
	${If} $PYINSTALLED == "1"
		CreateShortCut "$SMPROGRAMS\${SMNAME}\ASCEND.lnk" "$PYPATH\pythonw.exe" '"$INSTDIR\ascend"' "$INSTDIR\ascend.ico" 0
	${EndIf}

	; Model library shortcut
	CreateShortCut "$SMPROGRAMS\${SMNAME}\Model Library.lnk" "$INSTDIR\models" "" "$INSTDIR\models" 0

;	; Link to Tcl/Tk GUI  
;	${If} $TCLINSTALLED == "1"
;		CreateShortCut "$SMPROGRAMS\${SMNAME}\ASCEND Tcl/Tk.lnk" "$INSTDIR\ascend4.exe" "" "$INSTDIR\ascend4.exe" 0
;	${EndIf}
	
	; Documentation
	${If} $PDFINSTALLED == "1"
		CreateShortCut "$SMPROGRAMS\${SMNAME}\User's Manual.lnk" "$INSTDIR\book.pdf" "" "$INSTDIR\book.pdf" 0
	${EndIf}

	; Information files
	CreateShortCut "$SMPROGRAMS\${SMNAME}\LICENSE.lnk" "$INSTDIR\LICENSE.txt" '' "$INSTDIR\LICENSE.txt" 0
	CreateShortCut "$SMPROGRAMS\${SMNAME}\CHANGELOG.lnk" "$INSTDIR\CHANGELOG.txt" '' "$INSTDIR\CHANGELOG.txt" 0
	CreateShortCut "$SMPROGRAMS\${SMNAME}\README.lnk" "$INSTDIR\README-windows.txt" '' "$INSTDIR\README-windows.txt" 0

	CreateShortCut "$SMPROGRAMS\${SMNAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

;------------------------------------------------------------------
; HEADER FILES for DEVELOPERS

Section /o "Header files (for developers)" sect_devel
	WriteRegDWORD HKLM "SOFTWARE\ASCEND" "HeaderFiles" 1

	SetOutPath $INSTDIR\include\ascend
	File /r /x .svn "..\ascend\*.h"
	
	; Create 'ascend-config.bat' batch file for launching the python script 'ascend-config'.
	ClearErrors
	FileOpen $0 $INSTDIR\ascend-config.bat w
	${If} ${Errors}
		MessageBox MB_OK "The 'ascend-config.bat' file was not installed properly; problems writing to that file."	
	${Else}
		FileWrite $0 "@echo off"
		FileWriteByte $0 "13"
		FileWriteByte $0 "10"
		FileWrite $0 "set PATH=$PATH"
		FileWriteByte $0 "13"
		FileWriteByte $0 "10"
		FileWrite $0 "cd "
		FileWrite $0 $INSTDIR 
		FileWriteByte $0 "13"
		FileWriteByte $0 "10"
		FileWrite $0 "$PYPATH\python "
		FileWriteByte $0 "34" 
		FileWrite $0 "$INSTDIR\ascend-config"
		FileWriteByte $0 "34"
		FileWrite $0 " %1 %2 %3 %4 %5 %6 %7 %8"
		FileWriteByte $0 "13"
		FileWriteByte $0 "10"
		FileClose $0
	${EndIf}
	SetOutPath $INSTDIR
SectionEnd

;------------------------------------------------------------------
; UNINSTALLER

Section "Uninstall"
!ifdef INST64
	SetRegView 64
!endif

;--- python components ---

	ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "Python"
	${If} $0 <> 0
  
		DetailPrint "--- REMOVING PYTHON COMPONENTS ---"
		Delete $INSTDIR\python\_ascpy.pyd
		Delete $INSTDIR\python\_fprops.pyd
		Delete $INSTDIR\python\*.py
		Delete $INSTDIR\python\*.pyc
		Delete $INSTDIR\glade\*.glade
		Delete $INSTDIR\glade\*.png
		Delete $INSTDIR\glade\*.svg
		Delete $INSTDIR\ascend-doc.ico
		Delete $INSTDIR\ascend
		Delete $INSTDIR\ascend.bat
		RmDir $INSTDIR\glade
		RmDir $INSTDIR\python

;--- file association (for Python GUI) ---
  
		DetailPrint "--- REMOVING FILE ASSOCIATION ---"
		;start of restore script
		ReadRegStr $1 HKCR ".a4c" ""
		${If} $1 == "ASCEND.model"
			ReadRegStr $1 HKLM "SOFTWARE\ASCEND" "BackupAssocA4C"
			${If} $1 == ""
				; nothing to restore: delete it
				DeleteRegKey HKCR ".a4c"
			${Else}
				WriteRegStr HKCR ".a4c" "" $1
			${EndIf}
			DeleteRegValue HKLM "SOFTWARE\ASCEND" "BackupAssocA4C"
		${EndIf}

		ReadRegStr $1 HKCR ".a4l" ""	
		${If} $1 == "ASCEND.model"
			ReadRegStr $1 HKLM "SOFTWARE\ASCEND" "BackupAssocA4L"
			${If} $1 == ""
				; nothing to restore: delete it
				DeleteRegKey HKCR ".a4l"
			${Else}
				WriteRegStr HKCR ".a4l" "" $1
			${EndIf}
			DeleteRegValue HKLM "SOFTWARE\ASCEND" "BackupAssocA4L"
		${EndIf}

		DeleteRegKey HKCR "ASCEND.model" ;Delete key with association settings

		System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
		;rest of script

	${EndIf}

;--- tcl/tk components ---

;	ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "TclTk"
;	${If} $0 <> 0
;		DetailPrint "--- REMOVING TCL/TK COMPONENTS ---"
;		Delete $INSTDIR\ascendtcl.dll
;		Delete $INSTDIR\ascend4.exe
;		RMDir /r $INSTDIR\tcltk
;	${EndIf}

;--- documentation ---

	ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "PDF"
	${If} $0 <> 0
		DetailPrint "--- REMOVING DOCUMENTATION ---"
		Delete $INSTDIR\book.pdf
	${EndIf}

;--- header files ---

	ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "HeaderFiles"
	${If} $0 <> 0
		DetailPrint "--- REMOVING HEADER FILES ---"
		RMDir /r $INSTDIR\include
	${EndIf}
	
;--- start menu ---

	ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "StartMenu"
	${If} $0 <> 0
		; Remove shortcuts, if any
		DetailPrint "--- REMOVING START MENU SHORTCUTS ---"
		RmDir /r "$SMPROGRAMS\${SMNAME}"
	${EndIf}

;--- common components ---

	DetailPrint "--- REMOVING COMMON COMPONENTS ---"
	; Remove registry keys

	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ASCEND"
	DeleteRegKey HKLM "SOFTWARE\ASCEND"

	; Remove files and uninstaller

	Delete $INSTDIR\ascend-config
	Delete $INSTDIR\ascend-config.bat	
	Delete $INSTDIR\ascend.dll
	Delete $INSTDIR\LICENSE.txt
	Delete $INSTDIR\README-windows.txt
	Delete $INSTDIR\CHANGELOG.txt
	Delete $INSTDIR\ascend.ico
	Delete $INSTDIR\Makefile.bt
	Delete $INSTDIR\ascend.syn
	Delete $INSTDIR\ipopt38.dll
	RMDir /r $INSTDIR\models
	Delete $INSTDIR\solvers\qrslv_ascend.dll
	Delete $INSTDIR\solvers\conopt_ascend.dll
	Delete $INSTDIR\solvers\lrslv_ascend.dll
	Delete $INSTDIR\solvers\cmslv_ascend.dll
	Delete $INSTDIR\solvers\lsode_ascend.dll
	Delete $INSTDIR\solvers\ida_ascend.dll
	Delete $INSTDIR\solvers\dopri5_ascend.dll
	Delete $INSTDIR\solvers\ipopt_ascend.dll
	RMDir $INSTDIR\solvers

	${DEL_IPOPT_1}
	${DEL_IPOPT_2}
	${DEL_IPOPT_3}
	${DEL_IPOPT_4}
	${DEL_IPOPT_5}
	
	; Remove directories used

	Delete $INSTDIR\uninstall.exe
	RMDir $INSTDIR

SectionEnd

!include "dependencies.nsi"

!include "detect.nsi"

!include "ascendini.nsi"

!include "envvarwarning.nsi"

Function .onInit
!ifdef INST64
	${IfNot} ${RunningX64}
		MessageBox MB_OK "This ASCEND installer is for 64-bit Windows versions only.$\n$\nVisit http://ascend4.org for 32-bit versions."
		Abort
	${EndIf}
	SetRegView 64
!endif

	;Get the previously-chosen $INSTDIR
	ReadRegStr $0 HKLM "SOFTWARE\ASCEND" "Install_Dir"
	${If} $0 != ""
		StrCpy $INSTDIR $0
	${EndIf}

	;set the default python target dir
	StrCpy $PYTHONTARGETDIR "c:\Python${PYVERSION}"
!ifndef INST64
	${If} ${RunningX64}
		; this is a 32-bit installer on 64-bit Windows: install Python to a special location
		StrCpy $PYTHONTARGETDIR "c:\Python${PYVERSION}_32"
	${EndIf}
	; FIXME we should check whether that directory already exists before going ahead...
!endif
	StrCpy $GTK_GUI "NO"
	StrCpy $PYINSTALLED ""
	StrCpy $ASCENDINIFOUND ""
	StrCpy $PDFINSTALLED ""
	StrCpy $ASCENDENVVARFOUND ""
	
	ExpandEnvStrings $DEFAULTPATH "%WINDIR%;%WINDIR%\system32"

	Call DetectPython
	Call DetectGTK
	Call DetectGTKSourceView
	Call DetectDecorator
	Call DetectSimpleGeneric
	Call DetectGaphas
	
	;MessageBox MB_OK "GTK path is $GTKPATH"
	StrCpy $PATH "$DEFAULTPATH;$PYPATH"

	ReadRegStr $0 HKLM "SOFTWARE\ASCEND" "Install_Dir"
	${If} $0 != ""	
		;MessageBox MB_OK "Previous installation detected..."

		ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "Python"
		${If} $0 = 0
			;MessageBox MB_OK "Python was previously deselected"
			SectionGetFlags "${sect_pygtk}" $1
			IntOp $1 $1 ^ ${SF_SELECTED}
			SectionSetFlags "${sect_pygtk}" $1
		${Else}
			SectionGetFlags "${sect_pygtk}" $1
			IntOp $1 $1 ^ ${SF_RO}
			SectionSetFlags "${sect_pygtk}" $1		
		${EndIf}
		
		ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "Canvas"
		${If} $0 = 0
			;MessageBox MB_OK "Python was previously deselected"
			SectionGetFlags "${sect_canvas}" $1
			IntOp $1 $1 ^ ${SF_SELECTED}
			SectionSetFlags "${sect_canvas}" $1
		${Else}
			SectionGetFlags "${sect_canvas}" $1
			IntOp $1 $1 ^ ${SF_RO}
			SectionSetFlags "${sect_canvas}" $1		
		${EndIf}

		ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "PDF"
		${If} $0 = 0
			;MessageBox MB_OK "Documentation was previously deselected"
			SectionGetFlags "${sect_doc}" $1
			IntOp $1 $1 ^ ${SF_SELECTED}
			SectionSetFlags "${sect_doc}" $1
		${Else}
			SectionGetFlags "${sect_doc}" $1
			IntOp $1 $1 ^ ${SF_RO}
			SectionSetFlags "${sect_doc}" $1
		${EndIf}

		ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "StartMenu"
		${If} $0 = 0
			;MessageBox MB_OK "Start Menu was previously deselected"
			SectionGetFlags "${sect_menu}" $1
			IntOp $1 $1 ^ ${SF_SELECTED}
			SectionSetFlags "${sect_menu}" $1
		${Else}
			SectionGetFlags "${sect_menu}" $1
			IntOp $1 $1 ^ ${SF_RO}
			SectionSetFlags "${sect_menu}" $1
		${EndIf}
		
		ReadRegDWORD $0 HKLM "SOFTWARE\ASCEND" "HeaderFiles"
		${If} $0 <> 0
			;MessageBox MB_OK "Header files were previously selected"
			SectionGetFlags "${sect_devel}" $1
			IntOp $1 $1 | ${SF_SELECTED}
			IntOp $1 $1 | ${SF_RO}
			SectionSetFlags "${sect_devel}" $1
		${EndIf}
	${EndIf}	

FunctionEnd


Function un.onInit
!ifdef INST64
	SetRegView 64
!endif

	;Get the previously-chosen $INSTDIR
	ReadRegStr $0 HKLM "SOFTWARE\ASCEND" "Install_Dir"
	${If} $0 != ""
		StrCpy $INSTDIR $0
	${EndIf}
FunctionEnd

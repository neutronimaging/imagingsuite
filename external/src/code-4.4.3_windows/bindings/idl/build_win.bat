@ECHO OFF
REM  Copyright (c) 1988-2006, Research Systems Inc.  All rights reserved.
REM  This software includes information which is proprietary to and a
REM  trade secret of Research Systems, Inc.  It is not to be disclosed
REM  to anyone outside of this organization. Reproduction by any means
REM  whatsoever is  prohibited without express written permission.
REM
REM  MS Windows batch file to build the NeXusIDL-API DLM.
REM
REM  You may pass the location of the IDL directory to this file on the
REM  command line: e.g., build_win d:\myidl\idl
REM
REM  You may also edit the default location below.

SETLOCAL

IF "%1" == "" GOTO SET_IDLDIR
SET IDL_DIR=%1
GOTO CONTINUE

:SET_IDLDIR

REM Set these directories to point 

SET IDL_DIR=c:\rsi\idl63
SET NXS_DIR= "C:\Program Files\NeXus Data Format"

:CONTINUE

SET IDL_LIBDIR=%IDL_DIR%\bin\bin.x86
SET NXS_LIBDIR=%NXS_DIR%\lib

IF NOT EXIST %IDL_LIBDIR%\idl.lib GOTO NO_IDL_LIB
IF NOT EXIST %IDL_DIR%\external\include/idl_export.h GOTO NO_EXPORT_H

ECHO ON

cl -I%IDL_DIR%\external\include -nologo -DWIN32_LEAN_AND_MEAN -DWIN32 -c NeXusIDL-API.c
cl -I%IDL_DIR%\external\include -nologo -DWIN32_LEAN_AND_MEAN -DWIN32 -c handle.c

link /DLL /OUT:NeXusIDL-API.dll /DEF:NeXusIDL-API.def /IMPLIB:NeXusIDL-API.lib NeXusIDL-API.obj handle.obj %IDL_LIBDIR%\idl.lib %NXS_LIBDIR%\libNeXus.dll.lib


@ECHO OFF

GOTO END

:NO_IDL_LIB
ECHO.
ECHO Unable to locate %IDL_LIBDIR%\idl.lib.
ECHO.
GOTO END

:NO_EXPORT_H
ECHO.
ECHO Unable to locate %IDL_DIR%\external\include\idl_export.h.
ECHO.

:END

ENDLOCAL



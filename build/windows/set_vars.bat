@echo off
if not defined VSCMD_VER ( "%MSVCVARPATH%\vcvarsall.bat" amd64 )
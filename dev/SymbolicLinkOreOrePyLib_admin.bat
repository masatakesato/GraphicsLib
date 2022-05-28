@echo off
cd /d %~dp0

if not "%1"=="am_admin" (powershell start -verb runas '%0' am_admin & exit /b)

set src="..\..\oreorepylib\oreorepylib"
set target="..\external\oreorepylib"
if exist %target% (
    echo removing existing symboliclink... %target%
    rmdir %target%
)

echo creating symboliclink... %target%]
mklink /d %target% %src%

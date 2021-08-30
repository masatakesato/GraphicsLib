@echo off
cd /d %~dp0

set src="..\oreorelib\oreore"
set target=".\oreore"
if exist %target% (
    echo removing existing symboliclink... %target%
    rmdir %target%
)

echo creating symboliclink... %target%]
mklink /d %target% %src%

@echo off

start cmd /c %1
cd ./starcraft
start cmd /c "injectory_x86.exe --launch StarCraft.exe --inject bwapi-data/BWAPI.dll wmode.dll"

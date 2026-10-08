@echo off
set IP=127.0.0.1
set /p IP=Host IP (Enter = 127.0.0.1): 
start "" "NoGreedy.exe" %IP% -windowed -ResX=1280 -ResY=720

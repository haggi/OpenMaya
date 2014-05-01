@echo off

rem corona
set CORONA_MODULE=c:\<yourPathToCorona>\mayaToCorona
set MAYA_MODULE_PATH=%MAYA_MODULE_PATH%;%CORONA_MODULE%
set PATH=%CORONA_MODULE%\bin;%PATH%;

set PATH=C:\Program Files\Autodesk\Maya2014\bin;%PATH%

start maya.exe
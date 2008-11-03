@REM   HHC.EXE returns 1 for success and 0 for error
@REM   These values have to be reversed for Visual Studio
hhc.exe %1
@if errorlevel 1 exit 0 
@exit 1 

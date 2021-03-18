@ECHO OFF
REM DLL2LIB.BAT ------------------------------------------------------
REM     A batch script for generating importing LIB for DLL.
REM     ==========================================================
REM        Usage: dll2lib [32|64] some-file.dll
REM ------------------------------------------------------------------
SETLOCAL
if "%1"=="32" (set machine=x86) else (set machine=x64)
set dll_file=%2
set dll_file_no_ext=%dll_file:~0,-4%
set exports_file=%dll_file_no_ext%-exports.txt
set def_file=%dll_file_no_ext%.def
set lib_file=%dll_file_no_ext%.lib
set lib_name=%dll_file_no_ext%

REM -- load M$VC variables loader --
set VCPATH="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC"
call %VCPATH%\vcvarsall.bat
REM --------------------------------

echo "Dumping binary %dll_file% to %exports_file% ..."
dumpbin /exports %dll_file% > %exports_file%

echo LIBRARY %lib_name% > %def_file%
echo EXPORTS >> %def_file%
for /f "skip=19 tokens=1,4" %%A in (%exports_file%) do if NOT "%%B" == "" (echo %%B @%%A >> %def_file%)

lib /def:%def_file% /out:%lib_file% /machine:%machine%

REM Clean up temporary intermediate files
del %exports_file% %def_file% %dll_file_no_ext%.exp

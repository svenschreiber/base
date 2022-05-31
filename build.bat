@echo off

set project_name=app
set defines=-DBUILD_WIN32=1
if "%1"=="-release" (set optimization=-O2) else (set optimization=-Od)

set msvc=^"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall^" x64

set warning_exeptions=-wd4100 -wd4201
set common_compiler_flags= -MTd -GR -EHa-  %optimization% -Oi -W4 %warning_exeptions% -nologo -FC -Z7
set common_linker_flags=-incremental:no -opt:ref user32.lib gdi32.lib opengl32.lib

if not exist run_tree mkdir run_tree
pushd run_tree
if not defined DevEnvDir call %msvc%
cl  %defines% %common_compiler_flags% ..\src\app.c /link %common_linker_flags% /out:%project_name%.exe
popd

if %ERRORLEVEL% EQU 0 (
   echo ^>^> run_tree\%project_name%.exe
   if "%1"=="-run" (start "" run_tree\%project_name%.exe)
)

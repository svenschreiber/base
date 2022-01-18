@echo off

if "%1"=="-release" (set optimization=-O2) else (set optimization=-Od)
set project_name=app

set defines=-DBUILD_WIN32=1

set warning_exeptions=-wd4100
set common_compiler_flags= -MTd -GR -EHa-  %optimization% -Oi -W4 %warning_exeptions% -nologo -FC -Z7
set common_linker_flags=-incremental:no -opt:ref user32.lib gdi32.lib opengl32.lib

if not exist run_tree mkdir run_tree
pushd run_tree
cl  %defines% %common_compiler_flags% ..\src\%project_name%.c /link %common_linker_flags% /out:%project_name%.exe
popd

if %ERRORLEVEL% EQU 0 (
   if "%1"=="-run" (start "" run_tree\%project_name%.exe)
)

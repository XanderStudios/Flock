::
:: $Notice: Xander Studios @ 2024
:: $Author: Amélie Heinrich
:: $Create Time: 2024-11-27 16:07:19
::
@echo off

WHERE cl
if %ERRORLEVEL% neq 0 (
    call vcvarsall x64
)

if not exist run_tree (
    mkdir run_tree
)

set debug=true

set rootDir=%cd%

set source=%rootDir%\src\*.c

set links=user32.lib

set output=-FeFlock.exe

set includeDirs=/I %rootDir%\include

if %debug%==true (
    set flags=-EHsc /std:c++17 -Zi -w /MP -FC -nologo /utf-8 -DNOMINMAX -DWIN32_LEAN_AND_MEAN
    set debugFlags=/DEBUG:FULL
) else (
    set flags=-EHsc /std:c++17 -Zi -w /MP -FC -nologo -Oi -fp:fast /utf-8 -DNOMINMAX -DWIN32_LEAN_AND_MEAN
    set debugFlags=/DEBUG:NONE
)

pushd run_tree
cl %includeDirs% %output% %flags% %source% /link /incremental %debugFlags% %linkDirs% %links%
popd

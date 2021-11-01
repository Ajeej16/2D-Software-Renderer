
@echo off

REM -MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4189 -wd4100 -DHANDMADE_INTERNAL=1 -DHANDMADE_SLOW=1 -DHANDMADE_WIN32=1 -FAsc -Z7 -Fmwin32_handmade.map

set CompilerFlags=-nologo -FAsc -Z7
set LinkerFlags=-opt:ref user32.lib gdi32.lib Winmm.lib

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

cl %CompilerFlags% ..\software_renderer\code\win32_software_renderer.cpp /link %LinkerFlags%

popd
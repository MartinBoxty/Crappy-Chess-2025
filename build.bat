@echo off

REM Check if an argument was passed
if "%1"=="" (
    echo Usage: build [target]
    goto :eof
)

set include_path_sdl3=../sdl3-devel-3.2.20-mingw/x86_64-w64-mingw32/include/
set include_path_sdl3_image=../SDL3_image-3.2.4/x86_64-w64-mingw32/include/
set lib_path_sdl3=../SDL3-devel-3.2.20-mingw/x86_64-w64-mingw32/lib
set lib_path_sdl3_image=../SDL3_image-3.2.4/x86_64-w64-mingw32/lib

REM Handle different build targets
if "%1"=="debug" (
    xcopy .\lib\SDL3.dll .\bin\debug\ /y
    xcopy .\lib\SDL3_image.dll .\bin\debug\ /y
    gcc -o bin\debug\chess.exe main.c -std=c11 ^
    -I%include_path_sdl3% -L%lib_path_sdl3% -lsdl3 ^
    -I%include_path_sdl3_image% -L%lib_path_sdl3_image% -lsdl3_image
    goto :eof
)

if "%1"=="test" (
    xcopy .\lib\SDL3.dll .\bin\test\ /y
    xcopy .\lib\SDL3_image.dll .\bin\test\ /y
    gcc -o bin\test\test.exe test.c -std=c11 ^
    -I%include_path_sdl3% -L%lib_path_sdl3% -lsdl3 ^
    -I%include_path_sdl3_image% -L%lib_path_sdl3_image% -lsdl3_image
    goto :eof
)

REM If none matched, show error
echo Unknown build target: %1
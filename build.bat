@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64

set DEFINES=/DATTACH_CONSOLE /DDEBUG_MODE /D_HAS_EXCEPTIONS=0 /D_CRT_SECURE_NO_WARNINGS
set DISABLE_SOME_WARNINGS=-wd4505 -wd4702 -wd4100 -wd4267 -wd4244

set COMPILER_OPTIONS=%DEFINES%               ^
                     -nologo                 ^
                     /GR-                    ^
					 -EHa-                   ^
					 -Oi                     ^
					 -W4                     ^
					 /std:c++20              ^
					 %DISABLE_SOME_WARNINGS%

rem Stack Guards (security cookies) disabled
rem Stack Probes disabled by forcing their inclusion only when function's local variables require more than 9999999 bytes.
set SPECIAL_COMPILER_OPTIONS=/GS- /Gs9999999
rem Stack immediatelly reserved and commited (1MB).
set SPECIAL_LINKER_OPTIONS=-stack:0x100000,0x100000

set LIBRARIES=kernel32.lib

cl main.cpp %COMPILER_OPTIONS% %SPECIAL_COMPILER_OPTIONS% -link -nodefaultlib -subsystem:windows %LIBRARIES% %SPECIAL_LINKER_OPTIONS%

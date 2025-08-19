@echo off
echo Building Desktop Icon Toggler...

REM Setup Visual Studio environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if %ERRORLEVEL% neq 0 (
    echo Visual Studio environment setup failed!
    pause
    exit /b 1
)

REM Create output directory
if not exist "bin" mkdir bin

REM Compile resource file
echo Compiling resources...
rc /fo resources\app.res resources\app.rc
if %ERRORLEVEL% neq 0 (
    echo Resource compilation failed!
    pause
    exit /b 1
)

REM Compile C++ files
echo Compiling C++ source files...
cl /EHsc /std:c++17 /I include ^
   src\main.cpp ^
   src\Application.cpp ^
   src\DesktopIconManager.cpp ^
   src\HotkeyManager.cpp ^
   src\SystemTrayManager.cpp ^
   src\SettingsWindow.cpp ^
   src\ConfigManager.cpp ^
   resources\app.res ^
   /Fe:bin\DesktopIconToggler.exe ^
   /link user32.lib shell32.lib advapi32.lib comctl32.lib gdi32.lib kernel32.lib ole32.lib /SUBSYSTEM:WINDOWS

if %ERRORLEVEL% neq 0 (
    echo Compilation failed!
    pause
    exit /b 1
)

REM Copy config file
copy config\settings.ini.template bin\settings.ini

echo.
echo Build completed successfully!
echo Executable location: bin\DesktopIconToggler.exe
echo.
pause

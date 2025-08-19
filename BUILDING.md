# Building Desktop Icon Toggler

## Quick Start

1. **Prerequisites**:
   - Windows 10/11
   - Visual Studio 2019 or 2022 with C++ tools
   - CMake 3.16+

2. **Build**:
   ```batch
   # Run the automated build script
   build.bat
   ```

3. **Run**:
   ```batch
   # After successful build
   build\bin\Release\DesktopIconToggler.exe
   ```

## Manual Build Steps

If the automated script doesn't work:

```batch
# Create build directory
mkdir build
cd build

# Configure project
cmake .. -G "Visual Studio 17 2022" -A x64

# Build release version
cmake --build . --config Release
```

## Creating the Application Icon

The application includes a Python script to create a simple icon:

```batch
# If you have Python and Pillow installed
python create_icon.py
```

Or create your own 32x32 pixel ICO file and place it at `resources/icon.ico`.

## Project Structure

```
toogle-desktopicon/
├── CMakeLists.txt          # Build configuration
├── build.bat               # Automated build script
├── README.md               # User documentation
├── BUILDING.md             # This file
├── create_icon.py          # Icon creation script
├── config/
│   └── settings.ini.template  # Default configuration
├── include/                # Header files
│   ├── Common.h
│   ├── Application.h
│   ├── DesktopIconManager.h
│   ├── HotkeyManager.h
│   ├── SystemTrayManager.h
│   ├── SettingsWindow.h
│   └── ConfigManager.h
├── src/                    # Source files
│   ├── main.cpp
│   ├── Application.cpp
│   ├── DesktopIconManager.cpp
│   ├── HotkeyManager.cpp
│   ├── SystemTrayManager.cpp
│   ├── SettingsWindow.cpp
│   └── ConfigManager.cpp
└── resources/              # Application resources
    ├── app.rc
    ├── app.manifest
    └── icon.ico
```

## Troubleshooting Build Issues

### CMake not found
- Install CMake from https://cmake.org/download/
- Add CMake to your system PATH

### Visual Studio not found
- Install Visual Studio with "Desktop development with C++" workload
- Or install Build Tools for Visual Studio

### Missing Windows SDK
- Install Windows 10/11 SDK through Visual Studio Installer

### Resource compilation errors
- Ensure `app.manifest` and `app.rc` are in the `resources/` directory
- If icon errors occur, comment out the icon line in `app.rc`

## Build Output

Successful build creates:
- `build/bin/Release/DesktopIconToggler.exe` - Main executable
- `build/bin/Release/settings.ini` - Default configuration file

## Testing the Application

1. Run the executable
2. Check system tray for the application icon
3. Try the default hotkey: Ctrl+Alt+D
4. Right-click tray icon to access settings

## Distribution

For distribution, copy these files together:
- `DesktopIconToggler.exe`
- `settings.ini` (optional - will be created if missing)

The application is self-contained and doesn't require additional DLLs on most Windows systems.

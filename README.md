# Desktop Icon Toggler

A lightweight C++ Windows application that allows you to quickly toggle desktop icons visibility on/off using customizable hotkeys and system tray integration.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)

## Features

### Core Functionality
- **Toggle Desktop Icons**: Show/hide all desktop icons with a single action
- **System Tray Integration**: Runs quietly in the system tray
- **Customizable Hotkeys**: Set your own key combinations (default: Ctrl+Alt+D)
- **Persistent Settings**: Saves your preferences and remembers last state

### User Interface
- **System Tray Icon**: Visual indicator of current desktop icon state
- **Context Menu**: Right-click for quick access to toggle, settings, and exit
- **Settings Window**: Clean, intuitive interface for configuration
- **Real-time Hotkey Capture**: Easy hotkey configuration with live feedback

### Configuration Options
- **Hotkey Customization**: Choose any combination of Ctrl, Alt, Shift, Win + key
- **Startup Options**: Option to start with Windows
- **Notifications**: Toggle balloon tip notifications
- **State Memory**: Remember desktop icon state between sessions

## System Requirements

- **Operating System**: Windows 10 or Windows 11
- **For Building**: Visual Studio 2019 or later with C++ development tools
- **Runtime**: No additional dependencies required

## Installation

### Option 1: Download Pre-built Binary
1. Download the latest release from the [Releases](https://github.com/gn027c/ToggleDesktopIcon/releases) page
2. Extract the files to your desired location
3. Run `DesktopIconToggler.exe`

### Option 2: Build from Source

#### Prerequisites
- Visual Studio 2019 or later with C++ development tools
- Windows 10/11 SDK

#### Build Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/gn027c/ToggleDesktopIcon.git
   cd ToggleDesktopIcon
   ```

2. Run the build script:
   ```batch
   build.bat
   ```

3. The executable will be created at: `bin/DesktopIconToggler.exe`

For detailed build instructions, see [BUILDING.md](BUILDING.md).

## Usage

### First Run
1. Run `DesktopIconToggler.exe`
2. The application will appear in the system tray
3. Default hotkey is **Ctrl+Alt+D**

### Basic Operations
- **Toggle Icons**: Press your configured hotkey or left-click the tray icon
- **Settings**: Right-click tray icon → Settings
- **Exit**: Right-click tray icon → Exit

### Configuring Hotkeys
1. Right-click the tray icon and select "Settings"
2. Click the "Capture" button
3. Press your desired key combination
4. Click "OK" to save

### Supported Key Combinations
- Any combination of modifier keys: Ctrl, Alt, Shift, Win
- Plus any letter, number, or function key
- At least one modifier key is required

## Configuration File

Settings are stored in `settings.ini` in the same directory as the executable:

```ini
[Hotkey]
Ctrl=1
Alt=1
Shift=0
Win=0
KeyCode=68

[Application]
StartWithWindows=0
ShowNotifications=1
RememberState=1
LastIconState=1
```

## Technical Details

### Architecture
- **Application Class**: Main application coordinator
- **DesktopIconManager**: Handles Windows API calls for icon visibility
- **HotkeyManager**: Manages global hotkey registration and capture
- **SystemTrayManager**: Handles system tray icon and context menu
- **SettingsWindow**: Provides configuration interface
- **ConfigManager**: Manages INI file configuration

### Windows API Usage
- Uses `FindWindow` and `FindWindowEx` to locate desktop ListView
- Calls `ShowWindow` to control icon visibility
- Registers global hotkeys with `RegisterHotKey`
- Implements system tray with `Shell_NotifyIcon`

## Troubleshooting

### Common Issues

**Hotkey not working:**
- Check if another application is using the same hotkey
- Try a different key combination
- Restart the application

**Desktop icons not toggling:**
- Ensure you're running on Windows 10/11
- Try running as administrator (though not typically required)
- Check Windows version compatibility

**Application won't start:**
- Verify all required DLL files are present
- Check Windows Event Viewer for error details
- Ensure Visual C++ Redistributable is installed

### Debug Mode
The application includes error handling and will show message boxes for critical errors.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

### Development Setup
1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2025 gn027c

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## Author

**gn027c** - [GitHub Profile](https://github.com/gn027c)

## Acknowledgments

- Thanks to the Windows API documentation and community
- Inspired by the need for a simple desktop customization tool

## Version History

- **v1.0.0**: Initial release with core functionality
  - Desktop icon toggle
  - Customizable hotkeys
  - System tray integration
  - Settings persistence

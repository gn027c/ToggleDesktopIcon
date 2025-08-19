# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-08-19

### Added
- Initial release of Desktop Icon Toggler
- Core functionality to toggle desktop icons visibility
- System tray integration with context menu
- Customizable hotkey support (default: Ctrl+Alt+D)
- Settings window for configuration
- Persistent settings storage in INI file
- Support for modifier keys: Ctrl, Alt, Shift, Win
- Startup with Windows option
- Notification system for user feedback
- State memory between sessions
- Clean, intuitive user interface
- Real-time hotkey capture functionality

### Features
- **Desktop Icon Toggle**: Show/hide all desktop icons instantly
- **System Tray**: Runs quietly in background with visual state indicator
- **Hotkey Customization**: Set any combination of modifier keys + letter/number/function key
- **Settings Persistence**: Automatically saves and restores user preferences
- **Windows Integration**: Optional startup with Windows
- **User-Friendly Interface**: Easy-to-use settings dialog with live hotkey capture

### Technical Details
- Built with C++ and Windows API
- Uses FindWindow/FindWindowEx for desktop ListView detection
- Global hotkey registration with RegisterHotKey
- System tray implementation with Shell_NotifyIcon
- INI file configuration management
- Windows 10/11 compatible
- No external dependencies required

### Build System
- Visual Studio 2019/2022 support
- Automated build script (build.bat)
- CMake configuration available
- Resource compilation for icon and manifest
- Clean project structure with separate headers and source files

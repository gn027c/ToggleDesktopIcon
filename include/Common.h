#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <objbase.h>
#include <shlobj.h>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

// Application constants
constexpr int WM_TRAYICON = WM_USER + 1;
constexpr int WM_HOTKEY_PRESSED = WM_USER + 2;
constexpr int WM_SETTINGS_CHANGED = WM_USER + 3;

constexpr int ID_TRAY_ICON = 1001;
constexpr int ID_MENU_TOGGLE = 1002;
constexpr int ID_MENU_SETTINGS = 1003;
constexpr int ID_MENU_EXIT = 1004;

constexpr int ID_HOTKEY_TOGGLE = 2001;

// Settings window controls
constexpr int ID_HOTKEY_CTRL = 3001;
constexpr int ID_HOTKEY_ALT = 3002;
constexpr int ID_HOTKEY_SHIFT = 3003;
constexpr int ID_HOTKEY_WIN = 3004;
constexpr int ID_HOTKEY_KEY = 3005;
constexpr int ID_BUTTON_CAPTURE = 3006;
constexpr int ID_BUTTON_OK = 3007;
constexpr int ID_BUTTON_CANCEL = 3008;
constexpr int ID_STATUS_TEXT = 3009;

// Application info
constexpr const wchar_t* APP_NAME = L"Desktop Icon Toggler";
constexpr const wchar_t* APP_VERSION = L"1.0.0";
constexpr const wchar_t* CONFIG_FILE = L"settings.ini";
constexpr const wchar_t* WINDOW_CLASS_NAME = L"DesktopIconTogglerClass";
constexpr const wchar_t* SETTINGS_CLASS_NAME = L"DesktopIconTogglerSettingsClass";

// Hotkey structure
struct HotkeyConfig {
    bool ctrl = true;
    bool alt = true;
    bool shift = false;
    bool win = false;
    UINT vkCode = 'D';
    
    UINT GetModifiers() const {
        UINT modifiers = 0;
        if (ctrl) modifiers |= MOD_CONTROL;
        if (alt) modifiers |= MOD_ALT;
        if (shift) modifiers |= MOD_SHIFT;
        if (win) modifiers |= MOD_WIN;
        return modifiers;
    }
    
    std::wstring ToString() const {
        std::wstring result;
        if (ctrl) result += L"Ctrl+";
        if (alt) result += L"Alt+";
        if (shift) result += L"Shift+";
        if (win) result += L"Win+";
        
        // Convert virtual key code to string
        wchar_t keyName[256];
        UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
        if (GetKeyNameText(scanCode << 16, keyName, 256)) {
            result += keyName;
        } else {
            result += static_cast<wchar_t>(vkCode);
        }
        
        return result;
    }
};

// Application state
enum class IconState {
    Visible,
    Hidden,
    Unknown
};

// Utility functions
inline std::wstring GetLastErrorString() {
    DWORD error = GetLastError();
    if (error == 0) return L"No error";
    
    LPWSTR buffer = nullptr;
    size_t size = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&buffer, 0, nullptr);
    
    std::wstring message(buffer, size);
    LocalFree(buffer);
    return message;
}

inline void ShowErrorMessage(const std::wstring& message, const std::wstring& title = L"Error") {
    MessageBox(nullptr, message.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
}

inline void ShowInfoMessage(const std::wstring& message, const std::wstring& title = L"Information") {
    MessageBox(nullptr, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
}

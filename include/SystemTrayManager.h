#pragma once

#include "Common.h"

class SystemTrayManager {
public:
    SystemTrayManager();
    ~SystemTrayManager();

    // Initialization
    bool Initialize(HWND targetWindow, HINSTANCE hInstance);
    void Cleanup();
    
    // Tray icon management
    bool CreateTrayIcon();
    bool RemoveTrayIcon();
    bool UpdateTrayIcon(IconState iconState);
    bool ShowBalloonTip(const std::wstring& title, const std::wstring& message, DWORD timeout = 3000);
    
    // Context menu
    bool ShowContextMenu(int x, int y);
    bool HandleMenuCommand(UINT commandId);
    
    // Message handling
    bool HandleTrayMessage(WPARAM wParam, LPARAM lParam);
    
    // State management
    bool IsInitialized() const;
    void SetIconState(IconState state);
    IconState GetIconState() const;
    
    // Callbacks
    void SetToggleCallback(std::function<void()> callback);
    void SetSettingsCallback(std::function<void()> callback);
    void SetExitCallback(std::function<void()> callback);

private:
    // Internal state
    HWND m_targetWindow;
    HINSTANCE m_hInstance;
    NOTIFYICONDATA m_notifyIconData;
    HMENU m_contextMenu;
    bool m_initialized;
    IconState m_currentIconState;
    
    // Callbacks
    std::function<void()> m_toggleCallback;
    std::function<void()> m_settingsCallback;
    std::function<void()> m_exitCallback;
    
    // Icon resources
    HICON m_iconVisible;
    HICON m_iconHidden;
    
    // Helper methods
    bool LoadIcons();
    void UnloadIcons();
    bool CreateContextMenu();
    void DestroyContextMenu();
    HICON CreateCustomIcon(bool visible);
    void UpdateMenuItemText();
    std::wstring GetToggleMenuText() const;
};

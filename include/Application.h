#pragma once

#include "Common.h"
#include "DesktopIconManager.h"
#include "HotkeyManager.h"
#include "SystemTrayManager.h"
#include "SettingsWindow.h"
#include "ConfigManager.h"

class Application {
public:
    Application();
    ~Application();

    // Application lifecycle
    bool Initialize(HINSTANCE hInstance);
    int Run();
    void Shutdown();
    
    // Singleton access
    static Application* GetInstance();

    // Message handling
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    // Initialization helpers
    bool CreateMainWindow();
    bool InitializeComponents();
    bool LoadConfiguration();
    bool SetupCallbacks();
    
    // Event handlers
    void OnToggleDesktopIcons();
    void OnShowSettings();
    void OnExit();
    void OnHotkeyPressed();
    void OnSettingsChanged();
    
    // Utility methods
    void ShowNotification(const std::wstring& message);
    void UpdateTrayIconState();
    bool RegisterWindowClass();
    
    // Application state
    HINSTANCE m_hInstance;
    HWND m_mainWindow;
    bool m_initialized;
    bool m_running;
    
    // Component managers
    std::unique_ptr<DesktopIconManager> m_desktopIconManager;
    std::unique_ptr<HotkeyManager> m_hotkeyManager;
    std::unique_ptr<SystemTrayManager> m_systemTrayManager;
    std::unique_ptr<SettingsWindow> m_settingsWindow;
    std::unique_ptr<ConfigManager> m_configManager;
    
    // Singleton instance
    static Application* s_instance;
};

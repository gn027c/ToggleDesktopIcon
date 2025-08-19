#pragma once

#include "Common.h"

// Forward declarations
class HotkeyManager;
class ConfigManager;

class SettingsWindow {
public:
    SettingsWindow();
    ~SettingsWindow();

    // Window management
    bool Create(HWND parentWindow, HINSTANCE hInstance);
    void Show();
    void Hide();
    void Destroy();
    bool IsVisible() const;
    
    // Configuration
    void SetHotkeyManager(HotkeyManager* hotkeyManager);
    void SetConfigManager(ConfigManager* configManager);
    
    // Data management
    void LoadSettings();
    void SaveSettings();
    void ResetToDefaults();
    
    // Message handling
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    // Window state
    HWND m_hwnd;
    HWND m_parentWindow;
    HINSTANCE m_hInstance;
    bool m_visible;
    
    // Managers
    HotkeyManager* m_hotkeyManager;
    ConfigManager* m_configManager;
    
    // Control handles
    HWND m_checkCtrl;
    HWND m_checkAlt;
    HWND m_checkShift;
    HWND m_checkWin;
    HWND m_editKey;
    HWND m_buttonCapture;
    HWND m_buttonOK;
    HWND m_buttonCancel;
    HWND m_statusText;
    HWND m_checkStartWithWindows;
    HWND m_checkShowNotifications;
    HWND m_checkRememberState;
    
    // State
    bool m_capturing;
    HotkeyConfig m_currentConfig;
    HotkeyConfig m_originalConfig;
    
    // Window creation and layout
    bool RegisterWindowClass();
    bool CreateControls();
    void LayoutControls();
    void SetupFonts();
    
    // Control management
    void UpdateControlsFromConfig();
    void UpdateConfigFromControls();
    void EnableControls(bool enabled);
    void UpdateStatusText(const std::wstring& text);
    
    // Event handlers
    void OnCommand(WPARAM wParam, LPARAM lParam);
    void OnKeyDown(WPARAM wParam, LPARAM lParam);
    void OnKeyUp(WPARAM wParam, LPARAM lParam);
    void OnClose();
    void OnDestroy();
    
    // Button handlers
    void OnCaptureButton();
    void OnOKButton();
    void OnCancelButton();
    
    // Hotkey capture
    void StartHotkeyCapture();
    void StopHotkeyCapture();
    void UpdateCapturedHotkey();
    
    // Validation
    bool ValidateSettings();
    std::wstring GetValidationError();
    
    // Utility
    std::wstring VirtualKeyToString(UINT vkCode);
    void CenterWindow();
};

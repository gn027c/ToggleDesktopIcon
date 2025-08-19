#pragma once

#include "Common.h"

class HotkeyManager {
public:
    HotkeyManager();
    ~HotkeyManager();

    // Initialization
    bool Initialize(HWND targetWindow);
    void Cleanup();
    
    // Hotkey registration
    bool RegisterHotkey(const HotkeyConfig& config);
    bool UnregisterHotkey();
    bool UpdateHotkey(const HotkeyConfig& config);
    
    // Current configuration
    HotkeyConfig GetCurrentConfig() const;
    bool IsHotkeyRegistered() const;
    
    // Hotkey capture for settings UI
    bool StartCapture();
    void StopCapture();
    bool IsCaptureActive() const;
    HotkeyConfig GetCapturedHotkey() const;
    
    // Message handling
    bool HandleKeyDown(WPARAM wParam, LPARAM lParam);
    bool HandleKeyUp(WPARAM wParam, LPARAM lParam);
    
    // Validation
    static bool IsValidHotkey(const HotkeyConfig& config);
    static std::wstring GetHotkeyValidationError(const HotkeyConfig& config);

private:
    // Internal state
    HWND m_targetWindow;
    HotkeyConfig m_currentConfig;
    bool m_hotkeyRegistered;
    bool m_initialized;
    
    // Capture state
    bool m_captureActive;
    HotkeyConfig m_capturedConfig;
    bool m_captureModifiersPressed[4]; // ctrl, alt, shift, win
    
    // Helper methods
    bool RegisterSystemHotkey();
    bool UnregisterSystemHotkey();
    void ResetCaptureState();
    void UpdateCaptureState(WPARAM wParam, bool keyDown);
    UINT VirtualKeyToScanCode(UINT vkCode);
};

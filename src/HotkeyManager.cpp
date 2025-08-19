#include "HotkeyManager.h"

HotkeyManager::HotkeyManager()
    : m_targetWindow(nullptr)
    , m_hotkeyRegistered(false)
    , m_initialized(false)
    , m_captureActive(false) {
    
    // Initialize default configuration
    m_currentConfig.ctrl = true;
    m_currentConfig.alt = true;
    m_currentConfig.shift = false;
    m_currentConfig.win = false;
    m_currentConfig.vkCode = 'D';
    
    ResetCaptureState();
}

HotkeyManager::~HotkeyManager() {
    Cleanup();
}

bool HotkeyManager::Initialize(HWND targetWindow) {
    if (!targetWindow || !IsWindow(targetWindow)) {
        return false;
    }
    
    m_targetWindow = targetWindow;
    m_initialized = true;
    return true;
}

void HotkeyManager::Cleanup() {
    StopCapture();
    UnregisterHotkey();
    m_targetWindow = nullptr;
    m_initialized = false;
}

bool HotkeyManager::RegisterHotkey(const HotkeyConfig& config) {
    if (!m_initialized || !IsValidHotkey(config)) {
        return false;
    }
    
    // Unregister existing hotkey if any
    UnregisterHotkey();
    
    m_currentConfig = config;
    
    if (RegisterSystemHotkey()) {
        m_hotkeyRegistered = true;
        return true;
    }
    
    return false;
}

bool HotkeyManager::UnregisterHotkey() {
    if (!m_hotkeyRegistered) {
        return true;
    }
    
    bool success = UnregisterSystemHotkey();
    m_hotkeyRegistered = false;
    return success;
}

bool HotkeyManager::UpdateHotkey(const HotkeyConfig& config) {
    return RegisterHotkey(config);
}

HotkeyConfig HotkeyManager::GetCurrentConfig() const {
    return m_currentConfig;
}

bool HotkeyManager::IsHotkeyRegistered() const {
    return m_hotkeyRegistered;
}

bool HotkeyManager::StartCapture() {
    if (!m_initialized) {
        return false;
    }
    
    m_captureActive = true;
    ResetCaptureState();
    return true;
}

void HotkeyManager::StopCapture() {
    m_captureActive = false;
    ResetCaptureState();
}

bool HotkeyManager::IsCaptureActive() const {
    return m_captureActive;
}

HotkeyConfig HotkeyManager::GetCapturedHotkey() const {
    return m_capturedConfig;
}

bool HotkeyManager::HandleKeyDown(WPARAM wParam, LPARAM lParam) {
    if (!m_captureActive) {
        return false;
    }
    
    UpdateCaptureState(wParam, true);
    return true;
}

bool HotkeyManager::HandleKeyUp(WPARAM wParam, LPARAM lParam) {
    if (!m_captureActive) {
        return false;
    }
    
    UpdateCaptureState(wParam, false);
    return true;
}

bool HotkeyManager::IsValidHotkey(const HotkeyConfig& config) {
    // Must have at least one modifier
    if (!config.ctrl && !config.alt && !config.shift && !config.win) {
        return false;
    }
    
    // Virtual key code must be valid
    if (config.vkCode < 0x08 || config.vkCode > 0xFE) {
        return false;
    }
    
    // Avoid problematic key combinations
    if (config.vkCode == VK_LWIN || config.vkCode == VK_RWIN ||
        config.vkCode == VK_LCONTROL || config.vkCode == VK_RCONTROL ||
        config.vkCode == VK_LMENU || config.vkCode == VK_RMENU ||
        config.vkCode == VK_LSHIFT || config.vkCode == VK_RSHIFT) {
        return false;
    }
    
    return true;
}

std::wstring HotkeyManager::GetHotkeyValidationError(const HotkeyConfig& config) {
    if (!config.ctrl && !config.alt && !config.shift && !config.win) {
        return L"At least one modifier key (Ctrl, Alt, Shift, or Win) must be selected.";
    }
    
    if (config.vkCode < 0x08 || config.vkCode > 0xFE) {
        return L"Invalid key code.";
    }
    
    if (config.vkCode == VK_LWIN || config.vkCode == VK_RWIN ||
        config.vkCode == VK_LCONTROL || config.vkCode == VK_RCONTROL ||
        config.vkCode == VK_LMENU || config.vkCode == VK_RMENU ||
        config.vkCode == VK_LSHIFT || config.vkCode == VK_RSHIFT) {
        return L"Cannot use modifier keys as the main key.";
    }
    
    return L"";
}

bool HotkeyManager::RegisterSystemHotkey() {
    if (!m_targetWindow) {
        return false;
    }
    
    UINT modifiers = m_currentConfig.GetModifiers();
    return RegisterHotKey(m_targetWindow, ID_HOTKEY_TOGGLE, modifiers, m_currentConfig.vkCode) != 0;
}

bool HotkeyManager::UnregisterSystemHotkey() {
    if (!m_targetWindow) {
        return false;
    }
    
    return UnregisterHotKey(m_targetWindow, ID_HOTKEY_TOGGLE) != 0;
}

void HotkeyManager::ResetCaptureState() {
    m_capturedConfig.ctrl = false;
    m_capturedConfig.alt = false;
    m_capturedConfig.shift = false;
    m_capturedConfig.win = false;
    m_capturedConfig.vkCode = 0;
    
    for (int i = 0; i < 4; i++) {
        m_captureModifiersPressed[i] = false;
    }
}

void HotkeyManager::UpdateCaptureState(WPARAM wParam, bool keyDown) {
    UINT vkCode = static_cast<UINT>(wParam);
    
    // Handle modifier keys
    switch (vkCode) {
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
            m_captureModifiersPressed[0] = keyDown;
            m_capturedConfig.ctrl = keyDown;
            break;
            
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
            m_captureModifiersPressed[1] = keyDown;
            m_capturedConfig.alt = keyDown;
            break;
            
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
            m_captureModifiersPressed[2] = keyDown;
            m_capturedConfig.shift = keyDown;
            break;
            
        case VK_LWIN:
        case VK_RWIN:
            m_captureModifiersPressed[3] = keyDown;
            m_capturedConfig.win = keyDown;
            break;
            
        default:
            // Handle regular keys
            if (keyDown && vkCode >= 0x08 && vkCode <= 0xFE) {
                // Only capture if it's not a modifier key
                if (vkCode != VK_CONTROL && vkCode != VK_LCONTROL && vkCode != VK_RCONTROL &&
                    vkCode != VK_MENU && vkCode != VK_LMENU && vkCode != VK_RMENU &&
                    vkCode != VK_SHIFT && vkCode != VK_LSHIFT && vkCode != VK_RSHIFT &&
                    vkCode != VK_LWIN && vkCode != VK_RWIN) {
                    m_capturedConfig.vkCode = vkCode;
                }
            }
            break;
    }
}

UINT HotkeyManager::VirtualKeyToScanCode(UINT vkCode) {
    return MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
}

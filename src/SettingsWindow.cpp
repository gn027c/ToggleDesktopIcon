#include "SettingsWindow.h"
#include "HotkeyManager.h"
#include "ConfigManager.h"

SettingsWindow::SettingsWindow()
    : m_hwnd(nullptr)
    , m_parentWindow(nullptr)
    , m_hInstance(nullptr)
    , m_visible(false)
    , m_hotkeyManager(nullptr)
    , m_configManager(nullptr)
    , m_capturing(false) {
    
    // Initialize control handles
    m_checkCtrl = nullptr;
    m_checkAlt = nullptr;
    m_checkShift = nullptr;
    m_checkWin = nullptr;
    m_editKey = nullptr;
    m_buttonCapture = nullptr;
    m_buttonOK = nullptr;
    m_buttonCancel = nullptr;
    m_statusText = nullptr;
    m_checkStartWithWindows = nullptr;
    m_checkShowNotifications = nullptr;
    m_checkRememberState = nullptr;
}

SettingsWindow::~SettingsWindow() {
    Destroy();
}

bool SettingsWindow::Create(HWND parentWindow, HINSTANCE hInstance) {
    if (m_hwnd) {
        return true; // Already created
    }
    
    m_parentWindow = parentWindow;
    m_hInstance = hInstance;
    
    if (!RegisterWindowClass()) {
        return false;
    }
    
    // Create the window
    m_hwnd = CreateWindowEx(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
        SETTINGS_CLASS_NAME,
        L"Settings - Desktop Icon Toggler",
        WS_POPUP | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        400, 350,
        parentWindow,
        nullptr,
        hInstance,
        this
    );
    
    if (!m_hwnd) {
        return false;
    }
    
    if (!CreateControls()) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
        return false;
    }
    
    LayoutControls();
    SetupFonts();
    CenterWindow();
    
    return true;
}

void SettingsWindow::Show() {
    if (!m_hwnd) {
        return;
    }
    
    LoadSettings();
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    m_visible = true;
}

void SettingsWindow::Hide() {
    if (!m_hwnd) {
        return;
    }
    
    StopHotkeyCapture();
    ShowWindow(m_hwnd, SW_HIDE);
    m_visible = false;
}

void SettingsWindow::Destroy() {
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    m_visible = false;
}

bool SettingsWindow::IsVisible() const {
    return m_visible && m_hwnd && IsWindowVisible(m_hwnd);
}

void SettingsWindow::SetHotkeyManager(HotkeyManager* hotkeyManager) {
    m_hotkeyManager = hotkeyManager;
}

void SettingsWindow::SetConfigManager(ConfigManager* configManager) {
    m_configManager = configManager;
}

void SettingsWindow::LoadSettings() {
    if (!m_configManager) {
        return;
    }
    
    m_currentConfig = m_configManager->GetHotkeyConfig();
    m_originalConfig = m_currentConfig;
    
    UpdateControlsFromConfig();
    
    // Load application settings
    CheckDlgButton(m_hwnd, ID_HOTKEY_CTRL + 10, m_configManager->GetStartWithWindows() ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_hwnd, ID_HOTKEY_CTRL + 11, m_configManager->GetShowNotifications() ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_hwnd, ID_HOTKEY_CTRL + 12, m_configManager->GetRememberState() ? BST_CHECKED : BST_UNCHECKED);
}

void SettingsWindow::SaveSettings() {
    if (!m_configManager) {
        return;
    }
    
    UpdateConfigFromControls();
    
    m_configManager->SetHotkeyConfig(m_currentConfig);
    
    // Save application settings
    m_configManager->SetStartWithWindows(IsDlgButtonChecked(m_hwnd, ID_HOTKEY_CTRL + 10) == BST_CHECKED);
    m_configManager->SetShowNotifications(IsDlgButtonChecked(m_hwnd, ID_HOTKEY_CTRL + 11) == BST_CHECKED);
    m_configManager->SetRememberState(IsDlgButtonChecked(m_hwnd, ID_HOTKEY_CTRL + 12) == BST_CHECKED);
    
    m_configManager->SaveSettings();
}

void SettingsWindow::ResetToDefaults() {
    m_currentConfig.ctrl = true;
    m_currentConfig.alt = true;
    m_currentConfig.shift = false;
    m_currentConfig.win = false;
    m_currentConfig.vkCode = 'D';
    
    UpdateControlsFromConfig();
    
    CheckDlgButton(m_hwnd, ID_HOTKEY_CTRL + 10, BST_UNCHECKED); // Start with Windows
    CheckDlgButton(m_hwnd, ID_HOTKEY_CTRL + 11, BST_CHECKED);   // Show notifications
    CheckDlgButton(m_hwnd, ID_HOTKEY_CTRL + 12, BST_CHECKED);   // Remember state
}

LRESULT CALLBACK SettingsWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    SettingsWindow* pThis = nullptr;
    
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (SettingsWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    } else {
        pThis = (SettingsWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }
    
    if (pThis) {
        return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT SettingsWindow::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND:
            OnCommand(wParam, lParam);
            return 0;
            
        case WM_KEYDOWN:
            if (m_capturing) {
                OnKeyDown(wParam, lParam);
                return 0;
            }
            break;
            
        case WM_KEYUP:
            if (m_capturing) {
                OnKeyUp(wParam, lParam);
                return 0;
            }
            break;
            
        case WM_CLOSE:
            OnClose();
            return 0;
            
        case WM_DESTROY:
            OnDestroy();
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool SettingsWindow::RegisterWindowClass() {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = SETTINGS_CLASS_NAME;

    return RegisterClassEx(&wc) != 0;
}

bool SettingsWindow::CreateControls() {
    // Hotkey group
    CreateWindow(L"BUTTON", L"Hotkey Configuration",
                 WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                 10, 10, 360, 120, m_hwnd, nullptr, m_hInstance, nullptr);

    // Modifier checkboxes
    m_checkCtrl = CreateWindow(L"BUTTON", L"Ctrl",
                               WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                               20, 35, 60, 20, m_hwnd, (HMENU)ID_HOTKEY_CTRL, m_hInstance, nullptr);

    m_checkAlt = CreateWindow(L"BUTTON", L"Alt",
                              WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                              90, 35, 60, 20, m_hwnd, (HMENU)ID_HOTKEY_ALT, m_hInstance, nullptr);

    m_checkShift = CreateWindow(L"BUTTON", L"Shift",
                                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                160, 35, 60, 20, m_hwnd, (HMENU)ID_HOTKEY_SHIFT, m_hInstance, nullptr);

    m_checkWin = CreateWindow(L"BUTTON", L"Win",
                              WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                              230, 35, 60, 20, m_hwnd, (HMENU)ID_HOTKEY_WIN, m_hInstance, nullptr);

    // Key input
    CreateWindow(L"STATIC", L"Key:",
                 WS_CHILD | WS_VISIBLE,
                 20, 65, 30, 20, m_hwnd, nullptr, m_hInstance, nullptr);

    m_editKey = CreateWindow(L"EDIT", L"",
                             WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
                             55, 63, 100, 24, m_hwnd, (HMENU)ID_HOTKEY_KEY, m_hInstance, nullptr);

    m_buttonCapture = CreateWindow(L"BUTTON", L"Capture",
                                   WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                   165, 63, 80, 24, m_hwnd, (HMENU)ID_BUTTON_CAPTURE, m_hInstance, nullptr);

    // Status text
    m_statusText = CreateWindow(L"STATIC", L"Press 'Capture' and then press your desired hotkey combination",
                                WS_CHILD | WS_VISIBLE,
                                20, 95, 340, 20, m_hwnd, (HMENU)ID_STATUS_TEXT, m_hInstance, nullptr);

    // Application settings group
    CreateWindow(L"BUTTON", L"Application Settings",
                 WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                 10, 140, 360, 100, m_hwnd, nullptr, m_hInstance, nullptr);

    m_checkStartWithWindows = CreateWindow(L"BUTTON", L"Start with Windows",
                                           WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                           20, 165, 150, 20, m_hwnd, (HMENU)(UINT_PTR)(ID_HOTKEY_CTRL + 10), m_hInstance, nullptr);

    m_checkShowNotifications = CreateWindow(L"BUTTON", L"Show notifications",
                                            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                            20, 190, 150, 20, m_hwnd, (HMENU)(UINT_PTR)(ID_HOTKEY_CTRL + 11), m_hInstance, nullptr);

    m_checkRememberState = CreateWindow(L"BUTTON", L"Remember last state",
                                        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                        20, 215, 150, 20, m_hwnd, (HMENU)(UINT_PTR)(ID_HOTKEY_CTRL + 12), m_hInstance, nullptr);

    // Buttons
    m_buttonOK = CreateWindow(L"BUTTON", L"OK",
                              WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                              200, 260, 80, 30, m_hwnd, (HMENU)(UINT_PTR)ID_BUTTON_OK, m_hInstance, nullptr);

    m_buttonCancel = CreateWindow(L"BUTTON", L"Cancel",
                                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                  290, 260, 80, 30, m_hwnd, (HMENU)(UINT_PTR)ID_BUTTON_CANCEL, m_hInstance, nullptr);

    return true;
}

void SettingsWindow::LayoutControls() {
    // Controls are already positioned in CreateControls()
    // This method can be used for dynamic resizing if needed
}

void SettingsWindow::SetupFonts() {
    // Use default system font for now
    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

    EnumChildWindows(m_hwnd, [](HWND hwnd, LPARAM lParam) -> BOOL {
        SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, TRUE);
        return TRUE;
    }, (LPARAM)hFont);
}

void SettingsWindow::UpdateControlsFromConfig() {
    CheckDlgButton(m_hwnd, ID_HOTKEY_CTRL, m_currentConfig.ctrl ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_hwnd, ID_HOTKEY_ALT, m_currentConfig.alt ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_hwnd, ID_HOTKEY_SHIFT, m_currentConfig.shift ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(m_hwnd, ID_HOTKEY_WIN, m_currentConfig.win ? BST_CHECKED : BST_UNCHECKED);

    SetWindowText(m_editKey, VirtualKeyToString(m_currentConfig.vkCode).c_str());
}

void SettingsWindow::UpdateConfigFromControls() {
    m_currentConfig.ctrl = IsDlgButtonChecked(m_hwnd, ID_HOTKEY_CTRL) == BST_CHECKED;
    m_currentConfig.alt = IsDlgButtonChecked(m_hwnd, ID_HOTKEY_ALT) == BST_CHECKED;
    m_currentConfig.shift = IsDlgButtonChecked(m_hwnd, ID_HOTKEY_SHIFT) == BST_CHECKED;
    m_currentConfig.win = IsDlgButtonChecked(m_hwnd, ID_HOTKEY_WIN) == BST_CHECKED;
}

void SettingsWindow::EnableControls(bool enabled) {
    EnableWindow(m_checkCtrl, enabled);
    EnableWindow(m_checkAlt, enabled);
    EnableWindow(m_checkShift, enabled);
    EnableWindow(m_checkWin, enabled);
    EnableWindow(m_editKey, enabled);
    EnableWindow(m_buttonCapture, enabled);
    EnableWindow(m_buttonOK, enabled);
}

void SettingsWindow::UpdateStatusText(const std::wstring& text) {
    SetWindowText(m_statusText, text.c_str());
}

void SettingsWindow::OnCommand(WPARAM wParam, LPARAM lParam) {
    UINT commandId = LOWORD(wParam);

    switch (commandId) {
        case ID_BUTTON_CAPTURE:
            OnCaptureButton();
            break;

        case ID_BUTTON_OK:
            OnOKButton();
            break;

        case ID_BUTTON_CANCEL:
            OnCancelButton();
            break;

        case ID_HOTKEY_CTRL:
        case ID_HOTKEY_ALT:
        case ID_HOTKEY_SHIFT:
        case ID_HOTKEY_WIN:
            UpdateConfigFromControls();
            break;
    }
}

void SettingsWindow::OnKeyDown(WPARAM wParam, LPARAM lParam) {
    if (m_hotkeyManager && m_capturing) {
        m_hotkeyManager->HandleKeyDown(wParam, lParam);
        UpdateCapturedHotkey();
    }
}

void SettingsWindow::OnKeyUp(WPARAM wParam, LPARAM lParam) {
    if (m_hotkeyManager && m_capturing) {
        m_hotkeyManager->HandleKeyUp(wParam, lParam);
        UpdateCapturedHotkey();
    }
}

void SettingsWindow::OnClose() {
    Hide();
}

void SettingsWindow::OnDestroy() {
    m_hwnd = nullptr;
    m_visible = false;
}

void SettingsWindow::OnCaptureButton() {
    if (m_capturing) {
        StopHotkeyCapture();
    } else {
        StartHotkeyCapture();
    }
}

void SettingsWindow::OnOKButton() {
    if (!ValidateSettings()) {
        std::wstring error = GetValidationError();
        ShowErrorMessage(error, L"Invalid Settings");
        return;
    }

    SaveSettings();

    // Update hotkey registration
    if (m_hotkeyManager) {
        m_hotkeyManager->UpdateHotkey(m_currentConfig);
    }

    Hide();
}

void SettingsWindow::OnCancelButton() {
    // Restore original configuration
    m_currentConfig = m_originalConfig;
    UpdateControlsFromConfig();
    Hide();
}

void SettingsWindow::StartHotkeyCapture() {
    if (!m_hotkeyManager) {
        return;
    }

    m_capturing = true;
    m_hotkeyManager->StartCapture();

    SetWindowText(m_buttonCapture, L"Stop");
    UpdateStatusText(L"Listening for hotkey... Press your desired key combination");
    EnableControls(false);
    EnableWindow(m_buttonCapture, true);
    EnableWindow(m_buttonCancel, true);

    // Set focus to the window to capture keys
    SetFocus(m_hwnd);
}

void SettingsWindow::StopHotkeyCapture() {
    if (!m_hotkeyManager) {
        return;
    }

    m_capturing = false;
    m_hotkeyManager->StopCapture();

    SetWindowText(m_buttonCapture, L"Capture");
    UpdateStatusText(L"Press 'Capture' and then press your desired hotkey combination");
    EnableControls(true);
}

void SettingsWindow::UpdateCapturedHotkey() {
    if (!m_hotkeyManager || !m_capturing) {
        return;
    }

    HotkeyConfig captured = m_hotkeyManager->GetCapturedHotkey();

    // Update the configuration if we have a valid key
    if (captured.vkCode != 0) {
        m_currentConfig = captured;
        UpdateControlsFromConfig();

        std::wstring status = L"Captured: " + m_currentConfig.ToString();
        UpdateStatusText(status);
    }
}

bool SettingsWindow::ValidateSettings() {
    UpdateConfigFromControls();
    return HotkeyManager::IsValidHotkey(m_currentConfig);
}

std::wstring SettingsWindow::GetValidationError() {
    return HotkeyManager::GetHotkeyValidationError(m_currentConfig);
}

std::wstring SettingsWindow::VirtualKeyToString(UINT vkCode) {
    wchar_t keyName[256];
    UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);

    if (GetKeyNameText(scanCode << 16, keyName, 256)) {
        return std::wstring(keyName);
    }

    // Fallback for special keys
    switch (vkCode) {
        case VK_SPACE: return L"Space";
        case VK_RETURN: return L"Enter";
        case VK_ESCAPE: return L"Escape";
        case VK_TAB: return L"Tab";
        case VK_BACK: return L"Backspace";
        case VK_DELETE: return L"Delete";
        case VK_INSERT: return L"Insert";
        case VK_HOME: return L"Home";
        case VK_END: return L"End";
        case VK_PRIOR: return L"Page Up";
        case VK_NEXT: return L"Page Down";
        default:
            if (vkCode >= 'A' && vkCode <= 'Z') {
                return std::wstring(1, static_cast<wchar_t>(vkCode));
            }
            return L"Unknown";
    }
}

void SettingsWindow::CenterWindow() {
    if (!m_hwnd) {
        return;
    }

    RECT windowRect;
    GetWindowRect(m_hwnd, &windowRect);

    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;

    // Get screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate center position
    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;

    // Make sure window is not off-screen
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + width > screenWidth) x = screenWidth - width;
    if (y + height > screenHeight) y = screenHeight - height;

    SetWindowPos(m_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

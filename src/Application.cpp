#include "Application.h"

Application* Application::s_instance = nullptr;

Application::Application()
    : m_hInstance(nullptr)
    , m_mainWindow(nullptr)
    , m_initialized(false)
    , m_running(false) {
    
    s_instance = this;
}

Application::~Application() {
    Shutdown();
    s_instance = nullptr;
}

bool Application::Initialize(HINSTANCE hInstance) {
    if (m_initialized) {
        return true;
    }
    
    m_hInstance = hInstance;
    
    // Initialize COM for shell operations
    if (FAILED(CoInitialize(nullptr))) {
        ShowErrorMessage(L"Failed to initialize COM library");
        return false;
    }
    
    // Create main window
    if (!CreateMainWindow()) {
        ShowErrorMessage(L"Failed to create main window");
        return false;
    }
    
    // Initialize components
    if (!InitializeComponents()) {
        ShowErrorMessage(L"Failed to initialize application components");
        return false;
    }
    
    // Load configuration
    if (!LoadConfiguration()) {
        ShowErrorMessage(L"Failed to load configuration");
        return false;
    }
    
    // Setup callbacks
    if (!SetupCallbacks()) {
        ShowErrorMessage(L"Failed to setup callbacks");
        return false;
    }
    
    m_initialized = true;
    return true;
}

int Application::Run() {
    if (!m_initialized) {
        return -1;
    }
    
    m_running = true;
    
    MSG msg;
    while (m_running && GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return static_cast<int>(msg.wParam);
}

void Application::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    m_running = false;
    
    // Save configuration before shutdown
    if (m_configManager) {
        m_configManager->SaveSettings();
    }
    
    // Cleanup components in reverse order
    m_settingsWindow.reset();
    m_systemTrayManager.reset();
    m_hotkeyManager.reset();
    m_desktopIconManager.reset();
    m_configManager.reset();
    
    // Destroy main window
    if (m_mainWindow) {
        DestroyWindow(m_mainWindow);
        m_mainWindow = nullptr;
    }
    
    // Cleanup COM
    CoUninitialize();
    
    m_initialized = false;
}

Application* Application::GetInstance() {
    return s_instance;
}

bool Application::CreateMainWindow() {
    if (!RegisterWindowClass()) {
        return false;
    }
    
    // Create a hidden main window for message handling
    m_mainWindow = CreateWindowEx(
        0,
        WINDOW_CLASS_NAME,
        APP_NAME,
        WS_OVERLAPPED,
        CW_USEDEFAULT, CW_USEDEFAULT,
        0, 0,
        nullptr,
        nullptr,
        m_hInstance,
        this
    );
    
    return m_mainWindow != nullptr;
}

bool Application::InitializeComponents() {
    // Create component managers
    m_configManager = std::make_unique<ConfigManager>();
    m_desktopIconManager = std::make_unique<DesktopIconManager>();
    m_hotkeyManager = std::make_unique<HotkeyManager>();
    m_systemTrayManager = std::make_unique<SystemTrayManager>();
    m_settingsWindow = std::make_unique<SettingsWindow>();
    
    // Initialize components
    if (!m_configManager->Initialize()) {
        return false;
    }
    
    if (!m_desktopIconManager->Initialize()) {
        return false;
    }
    
    if (!m_hotkeyManager->Initialize(m_mainWindow)) {
        return false;
    }
    
    if (!m_systemTrayManager->Initialize(m_mainWindow, m_hInstance)) {
        return false;
    }
    
    if (!m_settingsWindow->Create(m_mainWindow, m_hInstance)) {
        return false;
    }
    
    // Set up cross-component references
    m_settingsWindow->SetHotkeyManager(m_hotkeyManager.get());
    m_settingsWindow->SetConfigManager(m_configManager.get());
    
    return true;
}

bool Application::LoadConfiguration() {
    if (!m_configManager) {
        return false;
    }
    
    // Load hotkey configuration
    HotkeyConfig hotkeyConfig = m_configManager->GetHotkeyConfig();
    if (!m_hotkeyManager->RegisterHotkey(hotkeyConfig)) {
        ShowErrorMessage(L"Failed to register hotkey: " + hotkeyConfig.ToString());
        // Continue anyway with default hotkey
    }
    
    // Restore last icon state if configured
    if (m_configManager->GetRememberState()) {
        IconState lastState = m_configManager->GetLastIconState();
        if (lastState == IconState::Hidden) {
            m_desktopIconManager->HideDesktopIcons();
        } else {
            m_desktopIconManager->ShowDesktopIcons();
        }
    }
    
    // Update tray icon to reflect current state
    UpdateTrayIconState();
    
    return true;
}

bool Application::SetupCallbacks() {
    if (!m_systemTrayManager) {
        return false;
    }
    
    // Set up system tray callbacks
    m_systemTrayManager->SetToggleCallback([this]() { OnToggleDesktopIcons(); });
    m_systemTrayManager->SetSettingsCallback([this]() { OnShowSettings(); });
    m_systemTrayManager->SetExitCallback([this]() { OnExit(); });
    
    return true;
}

void Application::OnToggleDesktopIcons() {
    if (!m_desktopIconManager) {
        return;
    }
    
    bool success = m_desktopIconManager->ToggleDesktopIcons();
    if (!success) {
        ShowErrorMessage(L"Failed to toggle desktop icons");
        return;
    }
    
    // Update tray icon state
    UpdateTrayIconState();
    
    // Save current state
    if (m_configManager && m_configManager->GetRememberState()) {
        IconState currentState = m_desktopIconManager->GetCurrentState();
        m_configManager->SetLastIconState(currentState);
    }
    
    // Show notification if enabled
    if (m_configManager && m_configManager->GetShowNotifications()) {
        IconState currentState = m_desktopIconManager->GetCurrentState();
        std::wstring message = (currentState == IconState::Visible) ? 
            L"Desktop icons are now visible" : L"Desktop icons are now hidden";
        ShowNotification(message);
    }
}

void Application::OnShowSettings() {
    if (m_settingsWindow) {
        m_settingsWindow->Show();
    }
}

void Application::OnExit() {
    m_running = false;
    PostQuitMessage(0);
}

void Application::OnHotkeyPressed() {
    OnToggleDesktopIcons();
}

void Application::OnSettingsChanged() {
    // Reload configuration and update components
    LoadConfiguration();
}

void Application::ShowNotification(const std::wstring& message) {
    if (m_systemTrayManager) {
        m_systemTrayManager->ShowBalloonTip(APP_NAME, message);
    }
}

void Application::UpdateTrayIconState() {
    if (!m_systemTrayManager || !m_desktopIconManager) {
        return;
    }
    
    IconState currentState = m_desktopIconManager->GetCurrentState();
    m_systemTrayManager->UpdateTrayIcon(currentState);
}

bool Application::RegisterWindowClass() {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    
    return RegisterClassEx(&wc) != 0;
}

LRESULT CALLBACK Application::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Application* pThis = nullptr;
    
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (Application*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    } else {
        pThis = (Application*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }
    
    if (pThis) {
        return pThis->HandleMessage(hwnd, uMsg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT Application::HandleMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_HOTKEY:
            if (wParam == ID_HOTKEY_TOGGLE) {
                OnHotkeyPressed();
            }
            return 0;
            
        case WM_TRAYICON:
            if (m_systemTrayManager) {
                m_systemTrayManager->HandleTrayMessage(wParam, lParam);
            }
            return 0;
            
        case WM_COMMAND:
            if (m_systemTrayManager) {
                m_systemTrayManager->HandleMenuCommand(LOWORD(wParam));
            }
            return 0;
            
        case WM_SETTINGS_CHANGED:
            OnSettingsChanged();
            return 0;
            
        case WM_DESTROY:
            OnExit();
            return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

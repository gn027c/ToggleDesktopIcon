#include "SystemTrayManager.h"
#include <windowsx.h>

SystemTrayManager::SystemTrayManager()
    : m_targetWindow(nullptr)
    , m_hInstance(nullptr)
    , m_contextMenu(nullptr)
    , m_initialized(false)
    , m_currentIconState(IconState::Visible)
    , m_iconVisible(nullptr)
    , m_iconHidden(nullptr) {
    
    ZeroMemory(&m_notifyIconData, sizeof(NOTIFYICONDATA));
}

SystemTrayManager::~SystemTrayManager() {
    Cleanup();
}

bool SystemTrayManager::Initialize(HWND targetWindow, HINSTANCE hInstance) {
    if (!targetWindow || !IsWindow(targetWindow) || !hInstance) {
        return false;
    }
    
    m_targetWindow = targetWindow;
    m_hInstance = hInstance;
    
    // Load icons
    if (!LoadIcons()) {
        return false;
    }
    
    // Create context menu
    if (!CreateContextMenu()) {
        UnloadIcons();
        return false;
    }
    
    // Initialize NOTIFYICONDATA structure
    m_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    m_notifyIconData.hWnd = m_targetWindow;
    m_notifyIconData.uID = ID_TRAY_ICON;
    m_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_notifyIconData.uCallbackMessage = WM_TRAYICON;
    m_notifyIconData.hIcon = m_iconVisible;
    wcscpy_s(m_notifyIconData.szTip, APP_NAME);
    
    m_initialized = true;
    return CreateTrayIcon();
}

void SystemTrayManager::Cleanup() {
    RemoveTrayIcon();
    DestroyContextMenu();
    UnloadIcons();
    
    m_targetWindow = nullptr;
    m_hInstance = nullptr;
    m_initialized = false;
}

bool SystemTrayManager::CreateTrayIcon() {
    if (!m_initialized) {
        return false;
    }
    
    return Shell_NotifyIcon(NIM_ADD, &m_notifyIconData) != FALSE;
}

bool SystemTrayManager::RemoveTrayIcon() {
    if (!m_initialized) {
        return true;
    }
    
    return Shell_NotifyIcon(NIM_DELETE, &m_notifyIconData) != FALSE;
}

bool SystemTrayManager::UpdateTrayIcon(IconState iconState) {
    if (!m_initialized) {
        return false;
    }
    
    m_currentIconState = iconState;
    
    // Update icon
    m_notifyIconData.hIcon = (iconState == IconState::Visible) ? m_iconVisible : m_iconHidden;
    
    // Update tooltip
    std::wstring tooltip = APP_NAME;
    tooltip += L" - Icons ";
    tooltip += (iconState == IconState::Visible) ? L"Visible" : L"Hidden";
    wcscpy_s(m_notifyIconData.szTip, tooltip.c_str());
    
    // Update menu text
    UpdateMenuItemText();
    
    return Shell_NotifyIcon(NIM_MODIFY, &m_notifyIconData) != FALSE;
}

bool SystemTrayManager::ShowBalloonTip(const std::wstring& title, const std::wstring& message, DWORD timeout) {
    if (!m_initialized) {
        return false;
    }
    
    NOTIFYICONDATA nid = m_notifyIconData;
    nid.uFlags |= NIF_INFO;
    nid.dwInfoFlags = NIIF_INFO;
    nid.uTimeout = timeout;
    wcscpy_s(nid.szInfoTitle, title.c_str());
    wcscpy_s(nid.szInfo, message.c_str());
    
    return Shell_NotifyIcon(NIM_MODIFY, &nid) != FALSE;
}

bool SystemTrayManager::ShowContextMenu(int x, int y) {
    if (!m_initialized || !m_contextMenu) {
        return false;
    }
    
    // Set foreground window to ensure menu appears properly
    SetForegroundWindow(m_targetWindow);
    
    // Show context menu
    TrackPopupMenu(
        m_contextMenu,
        TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_LEFTALIGN,
        x, y,
        0,
        m_targetWindow,
        nullptr
    );
    
    // Post a message to close the menu when clicking elsewhere
    PostMessage(m_targetWindow, WM_NULL, 0, 0);
    
    return true;
}

bool SystemTrayManager::HandleMenuCommand(UINT commandId) {
    switch (commandId) {
        case ID_MENU_TOGGLE:
            if (m_toggleCallback) {
                m_toggleCallback();
            }
            return true;
            
        case ID_MENU_SETTINGS:
            if (m_settingsCallback) {
                m_settingsCallback();
            }
            return true;
            
        case ID_MENU_EXIT:
            if (m_exitCallback) {
                m_exitCallback();
            }
            return true;
            
        default:
            return false;
    }
}

bool SystemTrayManager::HandleTrayMessage(WPARAM wParam, LPARAM lParam) {
    if (wParam != ID_TRAY_ICON) {
        return false;
    }
    
    switch (lParam) {
        case WM_LBUTTONUP:
            // Left click - toggle icons
            if (m_toggleCallback) {
                m_toggleCallback();
            }
            return true;
            
        case WM_RBUTTONUP:
            // Right click - show context menu
            {
                POINT pt;
                GetCursorPos(&pt);
                ShowContextMenu(pt.x, pt.y);
            }
            return true;
            
        default:
            return false;
    }
}

bool SystemTrayManager::IsInitialized() const {
    return m_initialized;
}

void SystemTrayManager::SetIconState(IconState state) {
    m_currentIconState = state;
}

IconState SystemTrayManager::GetIconState() const {
    return m_currentIconState;
}

void SystemTrayManager::SetToggleCallback(std::function<void()> callback) {
    m_toggleCallback = callback;
}

void SystemTrayManager::SetSettingsCallback(std::function<void()> callback) {
    m_settingsCallback = callback;
}

void SystemTrayManager::SetExitCallback(std::function<void()> callback) {
    m_exitCallback = callback;
}

bool SystemTrayManager::LoadIcons() {
    // Create custom icons since we don't have resource files yet
    m_iconVisible = CreateCustomIcon(true);
    m_iconHidden = CreateCustomIcon(false);
    
    return m_iconVisible && m_iconHidden;
}

void SystemTrayManager::UnloadIcons() {
    if (m_iconVisible) {
        DestroyIcon(m_iconVisible);
        m_iconVisible = nullptr;
    }
    
    if (m_iconHidden) {
        DestroyIcon(m_iconHidden);
        m_iconHidden = nullptr;
    }
}

bool SystemTrayManager::CreateContextMenu() {
    m_contextMenu = CreatePopupMenu();
    if (!m_contextMenu) {
        return false;
    }
    
    // Add menu items
    AppendMenu(m_contextMenu, MF_STRING, ID_MENU_TOGGLE, GetToggleMenuText().c_str());
    AppendMenu(m_contextMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(m_contextMenu, MF_STRING, ID_MENU_SETTINGS, L"Settings...");
    AppendMenu(m_contextMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenu(m_contextMenu, MF_STRING, ID_MENU_EXIT, L"Exit");
    
    return true;
}

void SystemTrayManager::DestroyContextMenu() {
    if (m_contextMenu) {
        DestroyMenu(m_contextMenu);
        m_contextMenu = nullptr;
    }
}

HICON SystemTrayManager::CreateCustomIcon(bool visible) {
    // Create a simple 16x16 icon
    const int iconSize = 16;
    HDC hdc = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, iconSize, iconSize);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
    
    // Fill background
    RECT rect = {0, 0, iconSize, iconSize};
    FillRect(hdcMem, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    
    // Draw icon representation
    HPEN hPen = CreatePen(PS_SOLID, 1, visible ? RGB(0, 128, 0) : RGB(128, 128, 128));
    HPEN hOldPen = (HPEN)SelectObject(hdcMem, hPen);
    
    if (visible) {
        // Draw a simple folder icon for visible state
        Rectangle(hdcMem, 2, 6, 14, 14);
        Rectangle(hdcMem, 2, 4, 8, 6);
    } else {
        // Draw a crossed-out folder for hidden state
        Rectangle(hdcMem, 2, 6, 14, 14);
        Rectangle(hdcMem, 2, 4, 8, 6);
        MoveToEx(hdcMem, 2, 6, nullptr);
        LineTo(hdcMem, 14, 14);
        MoveToEx(hdcMem, 14, 6, nullptr);
        LineTo(hdcMem, 2, 14);
    }
    
    SelectObject(hdcMem, hOldPen);
    DeleteObject(hPen);
    
    // Create mask bitmap
    HBITMAP hMask = CreateBitmap(iconSize, iconSize, 1, 1, nullptr);
    
    // Create icon
    ICONINFO iconInfo;
    iconInfo.fIcon = TRUE;
    iconInfo.xHotspot = 0;
    iconInfo.yHotspot = 0;
    iconInfo.hbmMask = hMask;
    iconInfo.hbmColor = hBitmap;
    
    HICON hIcon = CreateIconIndirect(&iconInfo);
    
    // Cleanup
    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteObject(hMask);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdc);
    
    return hIcon;
}

void SystemTrayManager::UpdateMenuItemText() {
    if (!m_contextMenu) {
        return;
    }
    
    ModifyMenu(m_contextMenu, ID_MENU_TOGGLE, MF_BYCOMMAND | MF_STRING, 
               ID_MENU_TOGGLE, GetToggleMenuText().c_str());
}

std::wstring SystemTrayManager::GetToggleMenuText() const {
    return (m_currentIconState == IconState::Visible) ? L"Hide Desktop Icons" : L"Show Desktop Icons";
}

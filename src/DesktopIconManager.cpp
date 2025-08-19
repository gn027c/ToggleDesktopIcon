#include "DesktopIconManager.h"
#include <iostream>

DesktopIconManager::DesktopIconManager()
    : m_currentState(IconState::Unknown)
    , m_desktopListView(nullptr)
    , m_progman(nullptr)
    , m_shelldll_defview(nullptr) {
}

DesktopIconManager::~DesktopIconManager() {
    Cleanup();
}

bool DesktopIconManager::Initialize() {
    if (!FindDesktopWindows()) {
        return false;
    }
    
    UpdateCurrentState();
    return true;
}

void DesktopIconManager::Cleanup() {
    m_desktopListView = nullptr;
    m_progman = nullptr;
    m_shelldll_defview = nullptr;
    m_currentState = IconState::Unknown;
}

bool DesktopIconManager::ToggleDesktopIcons() {
    if (!ValidateDesktopWindows()) {
        if (!FindDesktopWindows()) {
            return false;
        }
    }
    
    bool currentlyVisible = IsDesktopIconsVisible();
    return SetDesktopIconVisibility(!currentlyVisible);
}

bool DesktopIconManager::ShowDesktopIcons() {
    if (!ValidateDesktopWindows()) {
        if (!FindDesktopWindows()) {
            return false;
        }
    }
    
    return SetDesktopIconVisibility(true);
}

bool DesktopIconManager::HideDesktopIcons() {
    if (!ValidateDesktopWindows()) {
        if (!FindDesktopWindows()) {
            return false;
        }
    }
    
    return SetDesktopIconVisibility(false);
}

IconState DesktopIconManager::GetCurrentState() const {
    return m_currentState;
}

bool DesktopIconManager::IsDesktopIconsVisible() const {
    if (!m_desktopListView || !IsWindow(m_desktopListView)) {
        return true; // Default to visible if we can't determine
    }
    
    LONG style = GetWindowLong(m_desktopListView, GWL_STYLE);
    return (style & WS_VISIBLE) != 0;
}

HWND DesktopIconManager::FindDesktopListView() {
    // Method 1: Try to find through Progman -> SHELLDLL_DefView -> SysListView32
    HWND progman = FindWindow(L"Progman", L"Program Manager");
    if (progman) {
        HWND shelldll_defview = FindWindowEx(progman, nullptr, L"SHELLDLL_DefView", nullptr);
        if (shelldll_defview) {
            HWND listview = FindWindowEx(shelldll_defview, nullptr, L"SysListView32", L"FolderView");
            if (listview) {
                return listview;
            }
        }
    }
    
    // Method 2: Try to find through WorkerW windows (Windows 10/11)
    HWND workerw = nullptr;
    do {
        workerw = FindWindowEx(nullptr, workerw, L"WorkerW", nullptr);
        if (workerw) {
            HWND shelldll_defview = FindWindowEx(workerw, nullptr, L"SHELLDLL_DefView", nullptr);
            if (shelldll_defview) {
                HWND listview = FindWindowEx(shelldll_defview, nullptr, L"SysListView32", L"FolderView");
                if (listview) {
                    return listview;
                }
            }
        }
    } while (workerw);
    
    return nullptr;
}

bool DesktopIconManager::SetDesktopIconVisibility(bool visible) {
    if (!m_desktopListView || !IsWindow(m_desktopListView)) {
        return false;
    }
    
    // Show or hide the desktop listview
    int showCmd = visible ? SW_SHOW : SW_HIDE;
    ShowWindow(m_desktopListView, showCmd);
    
    // Force a refresh of the desktop
    RefreshDesktop();
    
    // Update our state
    UpdateCurrentState();
    
    return true;
}

bool DesktopIconManager::RefreshDesktop() {
    // Refresh the desktop to ensure changes are visible
    InvalidateRect(nullptr, nullptr, TRUE);
    
    // Send a message to refresh the desktop
    HWND desktop = GetDesktopWindow();
    if (desktop) {
        InvalidateRect(desktop, nullptr, TRUE);
        UpdateWindow(desktop);
    }
    
    // Also try to refresh the shell
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    
    return true;
}

bool DesktopIconManager::FindDesktopWindows() {
    m_progman = FindWindow(L"Progman", L"Program Manager");
    if (!m_progman) {
        return false;
    }
    
    m_desktopListView = FindDesktopListView();
    if (!m_desktopListView) {
        return false;
    }
    
    // Find the SHELLDLL_DefView window
    HWND parent = GetParent(m_desktopListView);
    if (parent) {
        wchar_t className[256];
        if (GetClassName(parent, className, 256) && 
            wcscmp(className, L"SHELLDLL_DefView") == 0) {
            m_shelldll_defview = parent;
        }
    }
    
    return m_desktopListView != nullptr;
}

bool DesktopIconManager::ValidateDesktopWindows() {
    return m_desktopListView && IsWindow(m_desktopListView) &&
           m_progman && IsWindow(m_progman);
}

void DesktopIconManager::UpdateCurrentState() {
    if (IsDesktopIconsVisible()) {
        m_currentState = IconState::Visible;
    } else {
        m_currentState = IconState::Hidden;
    }
}

#pragma once

#include "Common.h"

class DesktopIconManager {
public:
    DesktopIconManager();
    ~DesktopIconManager();

    // Core functionality
    bool ToggleDesktopIcons();
    bool ShowDesktopIcons();
    bool HideDesktopIcons();
    
    // State management
    IconState GetCurrentState() const;
    bool IsDesktopIconsVisible() const;
    
    // Initialization and cleanup
    bool Initialize();
    void Cleanup();

private:
    // Windows API helpers
    HWND FindDesktopListView();
    bool SetDesktopIconVisibility(bool visible);
    bool RefreshDesktop();
    
    // State tracking
    IconState m_currentState;
    HWND m_desktopListView;
    HWND m_progman;
    HWND m_shelldll_defview;
    
    // Internal methods
    bool FindDesktopWindows();
    bool ValidateDesktopWindows();
    void UpdateCurrentState();
};

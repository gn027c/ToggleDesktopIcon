#pragma once

#include "Common.h"

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();

    // Initialization
    bool Initialize();
    bool LoadSettings();
    bool SaveSettings();
    
    // Hotkey configuration
    HotkeyConfig GetHotkeyConfig() const;
    void SetHotkeyConfig(const HotkeyConfig& config);
    
    // Application settings
    bool GetStartWithWindows() const;
    void SetStartWithWindows(bool enable);
    
    bool GetShowNotifications() const;
    void SetShowNotifications(bool enable);
    
    bool GetRememberState() const;
    void SetRememberState(bool enable);
    
    IconState GetLastIconState() const;
    void SetLastIconState(IconState state);
    
    // File operations
    std::wstring GetConfigFilePath() const;
    bool ConfigFileExists() const;
    bool CreateDefaultConfig();

private:
    // INI file operations
    std::wstring ReadIniString(const std::wstring& section, const std::wstring& key, const std::wstring& defaultValue);
    int ReadIniInt(const std::wstring& section, const std::wstring& key, int defaultValue);
    bool WriteIniString(const std::wstring& section, const std::wstring& key, const std::wstring& value);
    bool WriteIniInt(const std::wstring& section, const std::wstring& key, int value);
    
    // Path management
    std::wstring GetExecutableDirectory();
    
    // Settings data
    HotkeyConfig m_hotkeyConfig;
    bool m_startWithWindows;
    bool m_showNotifications;
    bool m_rememberState;
    IconState m_lastIconState;
    
    // File path
    std::wstring m_configFilePath;
    bool m_initialized;
};

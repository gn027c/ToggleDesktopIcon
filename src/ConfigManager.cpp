#include "ConfigManager.h"
#include <shlobj.h>
#include <filesystem>

ConfigManager::ConfigManager()
    : m_startWithWindows(false)
    , m_showNotifications(true)
    , m_rememberState(true)
    , m_lastIconState(IconState::Visible)
    , m_initialized(false) {
    
    // Set default hotkey configuration
    m_hotkeyConfig.ctrl = true;
    m_hotkeyConfig.alt = true;
    m_hotkeyConfig.shift = false;
    m_hotkeyConfig.win = false;
    m_hotkeyConfig.vkCode = 'D';
}

ConfigManager::~ConfigManager() {
    if (m_initialized) {
        SaveSettings();
    }
}

bool ConfigManager::Initialize() {
    m_configFilePath = GetExecutableDirectory() + L"\\" + CONFIG_FILE;
    
    if (!ConfigFileExists()) {
        if (!CreateDefaultConfig()) {
            return false;
        }
    }
    
    m_initialized = LoadSettings();
    return m_initialized;
}

bool ConfigManager::LoadSettings() {
    if (m_configFilePath.empty()) {
        return false;
    }
    
    // Load hotkey configuration
    m_hotkeyConfig.ctrl = ReadIniInt(L"Hotkey", L"Ctrl", 1) != 0;
    m_hotkeyConfig.alt = ReadIniInt(L"Hotkey", L"Alt", 1) != 0;
    m_hotkeyConfig.shift = ReadIniInt(L"Hotkey", L"Shift", 0) != 0;
    m_hotkeyConfig.win = ReadIniInt(L"Hotkey", L"Win", 0) != 0;
    m_hotkeyConfig.vkCode = ReadIniInt(L"Hotkey", L"KeyCode", 'D');
    
    // Load application settings
    m_startWithWindows = ReadIniInt(L"Application", L"StartWithWindows", 0) != 0;
    m_showNotifications = ReadIniInt(L"Application", L"ShowNotifications", 1) != 0;
    m_rememberState = ReadIniInt(L"Application", L"RememberState", 1) != 0;
    
    int lastState = ReadIniInt(L"Application", L"LastIconState", 1);
    m_lastIconState = (lastState == 1) ? IconState::Visible : IconState::Hidden;
    
    return true;
}

bool ConfigManager::SaveSettings() {
    if (m_configFilePath.empty()) {
        return false;
    }
    
    // Save hotkey configuration
    if (!WriteIniInt(L"Hotkey", L"Ctrl", m_hotkeyConfig.ctrl ? 1 : 0) ||
        !WriteIniInt(L"Hotkey", L"Alt", m_hotkeyConfig.alt ? 1 : 0) ||
        !WriteIniInt(L"Hotkey", L"Shift", m_hotkeyConfig.shift ? 1 : 0) ||
        !WriteIniInt(L"Hotkey", L"Win", m_hotkeyConfig.win ? 1 : 0) ||
        !WriteIniInt(L"Hotkey", L"KeyCode", m_hotkeyConfig.vkCode)) {
        return false;
    }
    
    // Save application settings
    if (!WriteIniInt(L"Application", L"StartWithWindows", m_startWithWindows ? 1 : 0) ||
        !WriteIniInt(L"Application", L"ShowNotifications", m_showNotifications ? 1 : 0) ||
        !WriteIniInt(L"Application", L"RememberState", m_rememberState ? 1 : 0) ||
        !WriteIniInt(L"Application", L"LastIconState", (m_lastIconState == IconState::Visible) ? 1 : 0)) {
        return false;
    }
    
    return true;
}

HotkeyConfig ConfigManager::GetHotkeyConfig() const {
    return m_hotkeyConfig;
}

void ConfigManager::SetHotkeyConfig(const HotkeyConfig& config) {
    m_hotkeyConfig = config;
}

bool ConfigManager::GetStartWithWindows() const {
    return m_startWithWindows;
}

void ConfigManager::SetStartWithWindows(bool enable) {
    m_startWithWindows = enable;
}

bool ConfigManager::GetShowNotifications() const {
    return m_showNotifications;
}

void ConfigManager::SetShowNotifications(bool enable) {
    m_showNotifications = enable;
}

bool ConfigManager::GetRememberState() const {
    return m_rememberState;
}

void ConfigManager::SetRememberState(bool enable) {
    m_rememberState = enable;
}

IconState ConfigManager::GetLastIconState() const {
    return m_lastIconState;
}

void ConfigManager::SetLastIconState(IconState state) {
    m_lastIconState = state;
}

std::wstring ConfigManager::GetConfigFilePath() const {
    return m_configFilePath;
}

bool ConfigManager::ConfigFileExists() const {
    return std::filesystem::exists(m_configFilePath);
}

bool ConfigManager::CreateDefaultConfig() {
    // Create the default configuration file
    HANDLE hFile = CreateFile(
        m_configFilePath.c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    
    const char* defaultConfig = 
        "[Hotkey]\r\n"
        "; Modifier keys (1 = enabled, 0 = disabled)\r\n"
        "Ctrl=1\r\n"
        "Alt=1\r\n"
        "Shift=0\r\n"
        "Win=0\r\n"
        "\r\n"
        "; Virtual key code for the main key (default: 68 = 'D')\r\n"
        "KeyCode=68\r\n"
        "\r\n"
        "[Application]\r\n"
        "; Start with Windows (1 = enabled, 0 = disabled)\r\n"
        "StartWithWindows=0\r\n"
        "\r\n"
        "; Show notifications (1 = enabled, 0 = disabled)\r\n"
        "ShowNotifications=1\r\n"
        "\r\n"
        "; Remember last state (1 = enabled, 0 = disabled)\r\n"
        "RememberState=1\r\n"
        "\r\n"
        "; Last known desktop icon state (1 = visible, 0 = hidden)\r\n"
        "LastIconState=1\r\n";
    
    DWORD bytesWritten;
    bool success = WriteFile(hFile, defaultConfig, strlen(defaultConfig), &bytesWritten, nullptr);
    CloseHandle(hFile);
    
    return success;
}

std::wstring ConfigManager::ReadIniString(const std::wstring& section, const std::wstring& key, const std::wstring& defaultValue) {
    wchar_t buffer[1024];
    DWORD result = GetPrivateProfileString(
        section.c_str(),
        key.c_str(),
        defaultValue.c_str(),
        buffer,
        1024,
        m_configFilePath.c_str()
    );
    
    return std::wstring(buffer);
}

int ConfigManager::ReadIniInt(const std::wstring& section, const std::wstring& key, int defaultValue) {
    return GetPrivateProfileInt(
        section.c_str(),
        key.c_str(),
        defaultValue,
        m_configFilePath.c_str()
    );
}

bool ConfigManager::WriteIniString(const std::wstring& section, const std::wstring& key, const std::wstring& value) {
    return WritePrivateProfileString(
        section.c_str(),
        key.c_str(),
        value.c_str(),
        m_configFilePath.c_str()
    ) != 0;
}

bool ConfigManager::WriteIniInt(const std::wstring& section, const std::wstring& key, int value) {
    return WriteIniString(section, key, std::to_wstring(value));
}

std::wstring ConfigManager::GetExecutableDirectory() {
    wchar_t path[MAX_PATH];
    GetModuleFileName(nullptr, path, MAX_PATH);
    
    std::wstring fullPath(path);
    size_t lastSlash = fullPath.find_last_of(L'\\');
    if (lastSlash != std::wstring::npos) {
        return fullPath.substr(0, lastSlash);
    }
    
    return L".";
}

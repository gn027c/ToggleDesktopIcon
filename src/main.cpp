#include "Application.h"
#include <memory>

// Check if another instance is already running
bool IsAnotherInstanceRunning() {
    HANDLE hMutex = CreateMutex(nullptr, TRUE, L"DesktopIconTogglerMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        if (hMutex) {
            CloseHandle(hMutex);
        }
        return true;
    }
    return false;
}

// Show error message and exit
void ShowErrorAndExit(const std::wstring& message) {
    MessageBox(nullptr, message.c_str(), L"Desktop Icon Toggler - Error", MB_OK | MB_ICONERROR);
    ExitProcess(1);
}

// Windows application entry point
int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
    
    // Check for another instance
    if (IsAnotherInstanceRunning()) {
        ShowErrorAndExit(L"Desktop Icon Toggler is already running.\n\nCheck the system tray for the application icon.");
    }
    
    // Create and initialize application
    std::unique_ptr<Application> app = std::make_unique<Application>();
    
    if (!app->Initialize(hInstance)) {
        ShowErrorAndExit(L"Failed to initialize Desktop Icon Toggler.\n\nPlease check that you have the necessary permissions and try again.");
    }
    
    // Run the application
    int exitCode = app->Run();
    
    // Cleanup is handled by the Application destructor
    app.reset();
    
    return exitCode;
}

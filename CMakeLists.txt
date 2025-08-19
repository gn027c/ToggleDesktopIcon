cmake_minimum_required(VERSION 3.16)
project(DesktopIconToggler VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Windows specific settings
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE TRUE)
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/Application.cpp
    src/DesktopIconManager.cpp
    src/HotkeyManager.cpp
    src/SystemTrayManager.cpp
    src/SettingsWindow.cpp
    src/ConfigManager.cpp
)

# Header files
set(HEADERS
    include/Application.h
    include/DesktopIconManager.h
    include/HotkeyManager.h
    include/SystemTrayManager.h
    include/SettingsWindow.h
    include/ConfigManager.h
    include/Common.h
)

# Resource files
set(RESOURCES
    resources/app.rc
)

# Create executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})

# Link Windows libraries
target_link_libraries(${PROJECT_NAME}
    user32
    shell32
    advapi32
    comctl32
    gdi32
    kernel32
)

# Set subsystem to Windows (GUI application)
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    LINK_FLAGS "/SUBSYSTEM:WINDOWS"
)

# Copy config template to output directory
configure_file(
    ${CMAKE_SOURCE_DIR}/config/settings.ini.template
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/settings.ini
    COPYONLY
)

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/settings.ini
    DESTINATION bin
)

# Capture sources - just add files to parent target

# Common capture interface (always included)
list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/IGraphicsCapture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/IGraphicsCapture.cpp
)

# Windows-specific files (only compiled on Windows)
if(WIN32)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/WindowsGraphicsCapture.h
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/WindowsGraphicsCapture.cpp
    )
    list(APPEND PLATFORM_LIBS
        windowsapp
        shcore 
        psapi
        d3d11
        dxgi
    )
    message(STATUS "Including Windows Graphics Capture support")
endif()

# macOS-specific files (when implemented)
if(APPLE)
    # list(APPEND SOURCES
    #     ${CMAKE_CURRENT_SOURCE_DIR}/macos/MacGraphicsCapture.h
    #     ${CMAKE_CURRENT_SOURCE_DIR}/macos/MacGraphicsCapture.cpp
    # )
    # list(APPEND PLATFORM_LIBS
    #     "-framework AVFoundation"
    #     "-framework CoreMedia"
    # )
    message(STATUS "macOS capture support not implemented yet")
endif()

# Linux-specific files (when implemented) 
if(UNIX AND NOT APPLE)
    # list(APPEND SOURCES
    #     ${CMAKE_CURRENT_SOURCE_DIR}/linux/LinuxGraphicsCapture.h
    #     ${CMAKE_CURRENT_SOURCE_DIR}/linux/LinuxGraphicsCapture.cpp
    # )
    # list(APPEND PLATFORM_LIBS
    #     X11
    #     Xrandr
    # )
    message(STATUS "Linux capture support not implemented yet")
endif()

# Set variables for parent scope
set(SOURCES ${SOURCES} PARENT_SCOPE)
set(PLATFORM_LIBS ${PLATFORM_LIBS} PARENT_SCOPE)
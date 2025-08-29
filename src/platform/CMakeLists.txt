# Platform abstraction sources

# Common platform interface (always included)
list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/IWindow.h
	${CMAKE_CURRENT_SOURCE_DIR}/IWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IRenderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/IRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ITexture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ITexture.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/WindowFactory.h
    # ${CMAKE_CURRENT_SOURCE_DIR}/WindowFactory.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ImGuiManager.h
	${CMAKE_CURRENT_SOURCE_DIR}/ImGuiManager.cpp
)

# Windows-specific platform files
if(WIN32)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/Win32Window.h
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/Win32Window.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/D3D11Renderer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/D3D11Renderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/D3D11Texture.h
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/D3D11Texture.cpp
    )
    message(STATUS "Including Win32/DirectX11 platform support")
endif()

# macOS-specific platform files (when implemented)
if(APPLE)
    # list(APPEND SOURCES
    #     ${CMAKE_CURRENT_SOURCE_DIR}/macos/CocoaWindow.h
    #     ${CMAKE_CURRENT_SOURCE_DIR}/macos/CocoaWindow.cpp
    #     ${CMAKE_CURRENT_SOURCE_DIR}/macos/MetalRenderer.h
    #     ${CMAKE_CURRENT_SOURCE_DIR}/macos/MetalRenderer.cpp
    # )
    message(STATUS "macOS platform support not implemented yet")
endif()

# Linux-specific platform files (when implemented) 
if(UNIX AND NOT APPLE)
    # list(APPEND SOURCES
    #     ${CMAKE_CURRENT_SOURCE_DIR}/linux/X11Window.h
    #     ${CMAKE_CURRENT_SOURCE_DIR}/linux/X11Window.cpp
    #     ${CMAKE_CURRENT_SOURCE_DIR}/linux/OpenGLRenderer.h
    #     ${CMAKE_CURRENT_SOURCE_DIR}/linux/OpenGLRenderer.cpp
    # )
    message(STATUS "Linux platform support not implemented yet")
endif()

# Set variables for parent scope
set(SOURCES ${SOURCES} PARENT_SCOPE)
set(PLATFORM_LIBS ${PLATFORM_LIBS} PARENT_SCOPE)
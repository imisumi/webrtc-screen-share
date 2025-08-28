# ImGui library with platform-specific backends
set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
)

set(IMGUI_BACKENDS "")
set(IMGUI_LIBS "")

# Platform-specific ImGui backends and dependencies
if(WIN32)
    list(APPEND IMGUI_BACKENDS
        imgui/backends/imgui_impl_dx11.cpp
        imgui/backends/imgui_impl_win32.cpp
    )
    list(APPEND IMGUI_LIBS
        d3d11
        dxgi
    )
    message(STATUS "Including ImGui Win32/DirectX11 backends")
endif()

if(APPLE)
    # Build GLFW for macOS
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(glfw)
    
    list(APPEND IMGUI_BACKENDS
        imgui/backends/imgui_impl_metal.mm
        imgui/backends/imgui_impl_glfw.cpp
    )
    list(APPEND IMGUI_LIBS
        glfw
        "-framework Metal"
        "-framework MetalKit"
        "-framework Cocoa"
    )
    message(STATUS "Including ImGui GLFW/Metal backends")
endif()

if(UNIX AND NOT APPLE)
    # Build GLFW for Linux
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(glfw)
    
    # Find OpenGL for Linux
    find_package(OpenGL REQUIRED)
    
    list(APPEND IMGUI_BACKENDS
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/backends/imgui_impl_glfw.cpp
    )
    list(APPEND IMGUI_LIBS
        glfw
        OpenGL::GL
        ${CMAKE_DL_LIBS}  # For OpenGL loader
    )
    message(STATUS "Including ImGui GLFW/OpenGL backends")
endif()

# Create ImGui library
add_library(imgui STATIC
    ${IMGUI_SOURCES}
    ${IMGUI_BACKENDS}
)

target_include_directories(imgui PUBLIC
    imgui/
    imgui/backends/
)

target_link_libraries(imgui PUBLIC
    ${IMGUI_LIBS}
)

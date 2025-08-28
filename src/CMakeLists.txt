# Source CMakeLists.txt - handles all source compilation

# Core application files (always compiled)
set(SOURCES
    main.cpp
    App.cpp
    App.h
)

# Add subdirectories (they append to SOURCES and PLATFORM_LIBS)
add_subdirectory(platform)
add_subdirectory(capture)

# Platform-specific definitions (inherited from root CMakeLists.txt)
# WIN32, APPLE, UNIX are automatically available

# Create the executable with all collected sources
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries (core + platform-specific)
target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    ${PLATFORM_LIBS}  # Platform-specific libraries from subdirectories
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
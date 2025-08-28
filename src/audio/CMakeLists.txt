# Audio capture sources - example of how to add new features

# Common audio interface
list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/IAudioCapture.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AudioCaptureFactory.cpp
)

# Windows-specific audio files
if(WIN32)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/windows/WindowsAudioCapture.cpp
    )
    list(APPEND PLATFORM_LIBS
        winmm  # Windows multimedia API
    )
    message(STATUS "Including Windows Audio Capture support")
endif()

# macOS-specific audio files
if(APPLE)
    # list(APPEND SOURCES
    #     ${CMAKE_CURRENT_SOURCE_DIR}/macos/MacAudioCapture.cpp
    # )
    # list(APPEND PLATFORM_LIBS
    #     "-framework AudioToolbox"
    # )
endif()

# Set variables for parent scope
set(SOURCES ${SOURCES} PARENT_SCOPE)
set(PLATFORM_LIBS ${PLATFORM_LIBS} PARENT_SCOPE)
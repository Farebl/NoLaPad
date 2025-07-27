cmake_minimum_required(VERSION 3.22)

project(NoLaPad VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_PREFIX_PATH "/home/farebl/Qt/6.9.0/gcc_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)
find_package(CURL REQUIRED)
find_library(LAME_LIBRARY NAMES mp3lame lame REQUIRED PATHS /usr/lib /usr/local/lib NO_DEFAULT_PATH)
if(NOT LAME_LIBRARY)
    message(FATAL_ERROR "LAME library not found. Please install libmp3lame-dev.")
endif()

set(JUCE_DIR "${CMAKE_SOURCE_DIR}/JUCE")
if(NOT EXISTS "${JUCE_DIR}")
    message(FATAL_ERROR "JUCE directory not found at ${JUCE_DIR}. Please set JUCE_DIR correctly.")
endif()
add_subdirectory(${JUCE_DIR} JUCE)
add_definitions(-DJUCE_DISABLE_ASSERTIONS=1)

set(PROJECT_SOURCES
    main.cpp
    Project.cpp
    Project.h
    Track.h Track.cpp
    TrackSettings.h TrackSettings.cpp
    MicroTimer.h MicroTimer.cpp
    Metronome.h Metronome.cpp
    MetronomeSettings.h MetronomeSettings.cpp
    REC.h REC.cpp
    RECButton.h RECButton.cpp
    BPM.h BPM.cpp
    BeatCheckBox.h BeatCheckBox.cpp
    Fader.h Fader.cpp
    Potentiometer.h Potentiometer.cpp
    LoopButton.h LoopButton.cpp
    TrackColorButtons.h TrackColorButtons.cpp
    ClickableLabel.h ClickableLabel.cpp
    AudioSampleSelector.h AudioSampleSelector.cpp
    TransportCallback.h TransportCallback.cpp
    DigitalClockFace.h DigitalClockFace.cpp
    TrackRECButton.h TrackRECButton.cpp
)

qt_add_executable(NoLaPad
    ${PROJECT_SOURCES}
)

target_compile_options(NoLaPad PRIVATE -g -O0)  # Додано прапори для налагодження
target_compile_definitions(NoLaPad PRIVATE
    JUCE_USE_MP3AUDIOFORMAT=1
)

target_link_libraries(NoLaPad PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    CURL::libcurl
    ${LAME_LIBRARY}
)

target_include_directories(NoLaPad PRIVATE
    ${CMAKE_SOURCE_DIR}/JUCE/modules
    /usr/include
)

set_target_properties(NoLaPad PROPERTIES
    WIN32_EXECUTABLE FALSE
)

include(GNUInstallDirs)
install(TARGETS NoLaPad
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(Qt6_FOUND)
    qt_finalize_executable(NoLaPad)
endif()

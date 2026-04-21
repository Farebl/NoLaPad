cmake_minimum_required(VERSION 3.22)

project(NoLaPad VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INSTALL_RPATH "${Qt6_DIR}/../../../lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)


find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(CURL REQUIRED)
find_library(LAME_LIBRARY NAMES mp3lame lame REQUIRED)
if(NOT LAME_LIBRARY)
    message(FATAL_ERROR "LAME library not found. Please install libmp3lame-dev.")
endif()

set(JUCE_DIR "${CMAKE_SOURCE_DIR}/JUCE")
if(NOT EXISTS "${JUCE_DIR}")
    message(FATAL_ERROR "JUCE directory not found at ${JUCE_DIR}. Please set JUCE_DIR correctly.")
endif()
set(JUCE_USE_HARFBUZZ_SUBSET OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${JUCE_DIR} JUCE)
add_definitions(-DJUCE_DISABLE_ASSERTIONS=1)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(nlohmann/json)

set(PROJECT_SOURCES
    main.cpp
    Project.cpp
    Project.h
    Track.h Track.cpp
    TrackSettings.h TrackSettings.cpp
    MicroTimer.h MicroTimer.cpp
    Metronome.h Metronome.cpp
    MetronomeSettings.h MetronomeSettings.cpp

    RecorderButton.h RecorderButton.cpp
    BPMCounter.h BPMCounter.cpp
    BeatCheckBox.h BeatCheckBox.cpp
    Fader.h Fader.cpp
    EffectsSwitcher.h EffectsSwitcher.cpp
    RedButton.h RedButton.cpp
    TrackColorButtons.h TrackColorButtons.cpp
    ClickableLabel.h ClickableLabel.cpp
    AudioSampleSelector.h AudioSampleSelector.cpp

    DigitalClockFace.h DigitalClockFace.cpp

)

qt_add_executable(NoLaPad
    ${PROJECT_SOURCES}
    LCDCounter.h LCDCounter.cpp
    LCDDisplay.h LCDDisplay.cpp
    ProjectManager.h ProjectManager.cpp
    IAudioEngine.h
    IPlayer.h
    IRecorder.h
    IStorage.h
    ProjectSaveParameters.h
    JUCETrackPlayer.h JUCETrackPlayer.cpp
    JUCEAudioEngine.h JUCEAudioEngine.cpp
    JUCERecorder.h JUCERecorder.cpp
    AudioBlockInfo.h
    JUCEMetronomePlayer.h JUCEMetronomePlayer.cpp
    ITrackPlayer.h
    IMetronomePlayer.h
    ProjectView.h ProjectView.cpp
    JSONStorage.h JSONStorage.cpp
    Effects.h


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
    juce::juce_dsp
    CURL::libcurl
    nlohmann_json::nlohmann_json
    ${LAME_LIBRARY}
)
target_link_libraries(NoLaPad PRIVATE Qt6::Core)
target_link_libraries(NoLaPad PRIVATE Qt6::Core)
target_link_libraries(NoLaPad PRIVATE Qt6::Core Qt6::Widgets)

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





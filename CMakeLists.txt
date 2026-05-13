cmake_minimum_required(VERSION 3.22)
project(NoLaPad VERSION 3.1 LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_PROGRAM}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia)
find_package(CURL REQUIRED)
find_library(LAME_LIBRARY NAMES mp3lame lame REQUIRED)
if(NOT LAME_LIBRARY)
    message(FATAL_ERROR "LAME library not found. Run: sudo apt install libmp3lame-dev")
endif()

# ── FetchContent: JUCE ────────────────────────────────────────────────────────
include(FetchContent)

set(JUCE_BUILD_EXTRAS          OFF CACHE BOOL "" FORCE)
set(JUCE_BUILD_EXAMPLES        OFF CACHE BOOL "" FORCE)
set(JUCE_USE_HARFBUZZ_SUBSET   OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# ── FetchContent: nlohmann/json ───────────────────────────────────────────────
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install    OFF CACHE INTERNAL "")

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# ── Вихідні файли ─────────────────────────────────────────────────────────────
qt_add_executable(NoLaPad
    main.cpp
    resources.qrc
    Project.cpp           Project.h
    Track.cpp             Track.h
    TrackSettings.cpp     TrackSettings.h
    MicroTimer.cpp        MicroTimer.h
    Metronome.cpp         Metronome.h
    MetronomeSettings.cpp MetronomeSettings.h
    RecorderButton.cpp    RecorderButton.h
    BPMCounter.cpp        BPMCounter.h
    BeatCheckBox.cpp      BeatCheckBox.h
    Fader.cpp             Fader.h
    EffectsSwitcher.cpp   EffectsSwitcher.h
    RedButton.cpp         RedButton.h
    TrackColorButtons.cpp TrackColorButtons.h
    ClickableLabel.cpp    ClickableLabel.h
    AudioSampleSelector.cpp AudioSampleSelector.h
    DigitalClockFace.cpp  DigitalClockFace.h
    LCDCounter.cpp        LCDCounter.h
    LCDDisplay.cpp        LCDDisplay.h
    ProjectManager.cpp    ProjectManager.h
    IAudioEngine.h
    IPlayer.h
    IRecorder.h
    IStorage.h
    ProjectSaveParameters.h
    JUCETrackPlayer.cpp     JUCETrackPlayer.h
    JUCEAudioEngine.cpp     JUCEAudioEngine.h
    JUCERecorder.cpp        JUCERecorder.h
    AudioBlockInfo.h
    JUCEMetronomePlayer.cpp JUCEMetronomePlayer.h
    ITrackPlayer.h
    IMetronomePlayer.h
    ProjectView.cpp       ProjectView.h
    JSONStorage.cpp       JSONStorage.h
    Effects.h
    ProjectSettings.cpp   ProjectSettings.h
    Potentiometer.cpp     Potentiometer.h
    ElidedLabel.cpp       ElidedLabel.h
    resources.qrc
)

add_custom_command(TARGET NoLaPad POST_BUILD
    COMMAND ln -sfn "${CMAKE_SOURCE_DIR}/music" "$<TARGET_FILE_DIR:NoLaPad>/music"
    COMMENT "Linking music/ into build directory"
)

target_compile_options(NoLaPad PRIVATE -g -O0)

target_compile_definitions(NoLaPad PRIVATE
    JUCE_USE_MP3AUDIOFORMAT=1
    JUCE_DISABLE_ASSERTIONS=1
)

target_link_libraries(NoLaPad PRIVATE
    Qt6::Core
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

target_include_directories(NoLaPad PRIVATE ${CMAKE_SOURCE_DIR})

set_target_properties(NoLaPad PROPERTIES WIN32_EXECUTABLE FALSE)

install(TARGETS NoLaPad RUNTIME DESTINATION bin)
qt_finalize_executable(NoLaPad)

add_subdirectory(tests)

# tests/integration/CMakeLists.txt

cmake_minimum_required(VERSION 3.22)

set(SRC_DIR        ${CMAKE_SOURCE_DIR})
set(MOCKS_DIR      ${CMAKE_SOURCE_DIR}/tests/mocks)
set(TESTS_UNIT_DIR ${CMAKE_SOURCE_DIR}/tests/unit)

function(add_qt_integration_test TARGET_NAME SOURCE_FILE EXTRA_SOURCES)

    qt_add_executable(${TARGET_NAME}
        ${SOURCE_FILE}
        ${EXTRA_SOURCES}
    )

    target_include_directories(${TARGET_NAME} PRIVATE
        ${SRC_DIR}
        ${MOCKS_DIR}
        ${SRC_DIR}/JUCE/modules
        /usr/include
    )

    target_link_libraries(${TARGET_NAME} PRIVATE
        Qt6::Test
        Qt6::Widgets
        Qt6::Multimedia
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_dsp
        nlohmann_json::nlohmann_json
        CURL::libcurl
    )

    target_compile_definitions(${TARGET_NAME} PRIVATE
        JUCE_USE_MP3AUDIOFORMAT=1
        JUCE_DISABLE_ASSERTIONS=1
        JUCE_USE_CURL=0
        JUCE_WEB_BROWSER=0
        JUCE_DISPLAY_NOTIFICATIONS=0
    )

    add_test(
        NAME    ${TARGET_NAME}
        COMMAND ${TARGET_NAME}
    )

endfunction()

# ─────────────────────────────────────────────────────────────────────────────
# Файли тестів беремо з tests/unit/ — там де вони фактично знаходяться
# ─────────────────────────────────────────────────────────────────────────────

# ── test_project ──────────────────────────────────────────────────────────────
# Потребує: Project + Track + усі GUI-віджети які Project/TrackSettings створюють
add_qt_integration_test(test_project
    ${TESTS_UNIT_DIR}/test_project.cpp
    "${SRC_DIR}/Project.cpp;\
${SRC_DIR}/Track.cpp;\
${SRC_DIR}/MicroTimer.cpp;\
${SRC_DIR}/Metronome.cpp;\
${SRC_DIR}/TrackSettings.cpp;\
${SRC_DIR}/RecorderButton.cpp;\
${SRC_DIR}/BPMCounter.cpp;\
${SRC_DIR}/BeatCheckBox.cpp;\
${SRC_DIR}/Fader.cpp;\
${SRC_DIR}/EffectsSwitcher.cpp;\
${SRC_DIR}/ElidedLabel.cpp;\
${SRC_DIR}/RedButton.cpp;\
${SRC_DIR}/TrackColorButtons.cpp;\
${SRC_DIR}/ClickableLabel.cpp;\
${SRC_DIR}/AudioSampleSelector.cpp;\
${SRC_DIR}/LCDCounter.cpp;\
${SRC_DIR}/LCDDisplay.cpp;\
${SRC_DIR}/DigitalClockFace.cpp;\
${SRC_DIR}/MetronomeSettings.cpp;\
${SRC_DIR}/JUCETrackPlayer.cpp;\
${SRC_DIR}/JUCEMetronomePlayer.cpp;\
${SRC_DIR}/Potentiometer.cpp"
)
 
# ── test_projectmanager ───────────────────────────────────────────────────────
# Потребує: ProjectManager + усі його залежності
# testHook_* методи мають бути додані у ProjectManager (див. ProjectManager.h)
add_qt_integration_test(test_projectmanager
    ${TESTS_UNIT_DIR}/test_projectmanager.cpp
    "${SRC_DIR}/ProjectManager.cpp;\
${SRC_DIR}/Project.cpp;\
${SRC_DIR}/Track.cpp;\
${SRC_DIR}/MicroTimer.cpp;\
${SRC_DIR}/Metronome.cpp;\
${SRC_DIR}/JSONStorage.cpp;\
${SRC_DIR}/TrackSettings.cpp;\
${SRC_DIR}/ProjectView.cpp;\
${SRC_DIR}/RecorderButton.cpp;\
${SRC_DIR}/BPMCounter.cpp;\
${SRC_DIR}/BeatCheckBox.cpp;\
${SRC_DIR}/Fader.cpp;\
${SRC_DIR}/EffectsSwitcher.cpp;\
${SRC_DIR}/ElidedLabel.cpp;\
${SRC_DIR}/RedButton.cpp;\
${SRC_DIR}/TrackColorButtons.cpp;\
${SRC_DIR}/ClickableLabel.cpp;\
${SRC_DIR}/AudioSampleSelector.cpp;\
${SRC_DIR}/LCDCounter.cpp;\
${SRC_DIR}/LCDDisplay.cpp;\
${SRC_DIR}/DigitalClockFace.cpp;\
${SRC_DIR}/MetronomeSettings.cpp;\
${SRC_DIR}/ProjectSettings.cpp;\
${SRC_DIR}/JUCETrackPlayer.cpp;\
${SRC_DIR}/JUCEMetronomePlayer.cpp;\
${SRC_DIR}/JUCEAudioEngine.cpp;\
${SRC_DIR}/JUCERecorder.cpp;\
${SRC_DIR}/Potentiometer.cpp"
)
 

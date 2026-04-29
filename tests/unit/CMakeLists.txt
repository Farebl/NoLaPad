# tests/unit/CMakeLists.txt

set(SRC_DIR ${CMAKE_SOURCE_DIR})
set(MOCKS_DIR ${CMAKE_SOURCE_DIR}/tests/mocks)


# ── Допоміжна функція для реєстрації тесту ──────────────────────────────────
function(add_qt_test TARGET_NAME SOURCE_FILE EXTRA_SOURCES)
    qt_add_executable(${TARGET_NAME}
        ${SOURCE_FILE}
        ${EXTRA_SOURCES}
    )

    target_include_directories(${TARGET_NAME} PRIVATE
        ${SRC_DIR}
        ${MOCKS_DIR}
        ${SRC_DIR}/JUCE/modules
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

    # Реєстрація в CTest
    add_test(
        NAME ${TARGET_NAME}
        COMMAND ${TARGET_NAME}
    )
endfunction()

# ── Реєстрація кожного тесту ─────────────────────────────────────────────────

add_qt_test(test_microtimer
    test_microtimer.cpp
    "${SRC_DIR}/MicroTimer.cpp"
)

add_qt_test(test_track
    test_track.cpp
    "${SRC_DIR}/Track.cpp;${SRC_DIR}/MicroTimer.cpp"
)

add_qt_test(test_jsonstorage
    test_jsonstorage.cpp
    "${SRC_DIR}/JSONStorage.cpp;${SRC_DIR}/ProjectView.cpp"
    
)

add_qt_test(test_jucerecorder
    test_jucerecorder.cpp
    "${SRC_DIR}/JUCERecorder.cpp"
)

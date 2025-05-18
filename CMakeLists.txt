cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME_CUSTOM "NoLapad")
project(${PROJECT_NAME_CUSTOM} VERSION 0.1 LANGUAGES CXX)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/home/farebl/Qt/6.9.0/gcc_64")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

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
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME_CUSTOM}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(${PROJECT_NAME_CUSTOM} SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(${PROJECT_NAME_CUSTOM}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(${PROJECT_NAME_CUSTOM} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME_CUSTOM})
endif()
set_target_properties(${PROJECT_NAME_CUSTOM} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

set(CMAKE_INSTALL_RPATH "/home/farebl/Qt/6.9.0/gcc_64/lib")

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME_CUSTOM}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME_CUSTOM})
endif()

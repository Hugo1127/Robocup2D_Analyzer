cmake_minimum_required(VERSION 3.16)

project(team_select2d VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(team_select2d
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        main.cpp
        autoplay/server.cpp
        autoplay/server.h
        autoplay/dialog.ui
        autoplay/dialog.cpp
        autoplay/dialog.h
        autoplay/autoplay.cpp
        autoplay/autoplay.h
        autoplay/autoplay.ui
        mainwindow.h
        mainwindow.ui
        mainwindow.cpp
        page.h
        page.cpp
        page.ui
        analysis/analysis.h analysis/analysis.cpp analysis/analysis.ui
        analysis/world_model.h analysis/world_model.cpp
        analysis/physics.h analysis/physics.cpp
        analysis/rcg2csv.h analysis/rcg2csv.cpp
        main.qrc
        design.qss


    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET team_select2d APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(team_select2d SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(team_select2d
            ${PROJECT_SOURCES}
        )
    endif()
endif()

#在此处添加包
target_link_libraries(team_select2d PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(team_select2d PRIVATE Qt${QT_VERSION_MAJOR}::Network)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.team_select2d)
endif()
set_target_properties(team_select2d PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS team_select2d
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(team_select2d)
endif()

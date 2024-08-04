cmake_minimum_required(VERSION 3.16)

project(LeeAutoExportFBX VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WIN32_EXECUTABLE TRUE)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui Core)

set(MELEXPORT ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/MayaExportCmd.mel)
set(BLENDEREXPORT ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/BlenderExport.py)

set(LEEDEFINE ${CMAKE_CURRENT_SOURCE_DIR}/include/Definitions.h)
set(MASSFBXLOG ${CMAKE_CURRENT_BINARY_DIR}/Logs/MassFbxLog.txt)

add_definitions(-MELEXPORT)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(PROJECT_SOURCES
    include/mainwindow.h
    include/commandline.h
    include/Definitions.h
    include/leespoiler.h

    src/main.cpp
    src/mainwindow.cpp
    src/commandline.cpp
    src/mainwindow.ui
    src/FbxOptions.ui
    src/leespoiler.cpp

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(LeeAutoExportFBX
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}


    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET LeeAutoExportFBX APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(LeeAutoExportFBX SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(LeeAutoExportFBX
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(LeeAutoExportFBX PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_compile_definitions(LeeAutoExportFBX PUBLIC MELEXPORT=${MELEXPORT})
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.LeeAutoExportFBX)
endif()
set_target_properties(LeeAutoExportFBX PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS LeeAutoExportFBX
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LeeAutoExportFBX)
endif()

if(NOT EXISTS(${LEEDEFINE}))
    FILE(WRITE ${LEEDEFINE}
    "#define MELEXPORTSCRIPT \"${MELEXPORT}\"\n"
    "#define MASSFBXLOG \"${MASSFBXLOG}\"\n"
    "#define BLENDEREXPORT \"${BLENDEREXPORT}\"\n"
    )
endif()

cmake_minimum_required(VERSION 3.16)

project(LeeAutoExportFBX VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WIN32_EXECUTABLE TRUE)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui Core Network)

set(SCRIPT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Scripts)

set(USER $ENV{USERNAME})
set(LOCALSCRIPT "C:/Users/${USER}/AppData/Local/LeeMassFbx/")
set(MELEXPORT ${LOCALSCRIPT}MayaExportCmd.mel)
set(BLENDEREXPORT ${LOCALSCRIPT}BlenderExport.py)

#Message(STATUS "${LOCALSCRIPT}")
Message(STATUS "Lee ${LOCALSCRIPT}")

set(LEEDEFINE ${CMAKE_CURRENT_SOURCE_DIR}/include/Definitions.h)
set(MASSFBXLOG ${LOCALSCRIPT}MassFbxLog.txt)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(PROJECT_SOURCES
    include/mainwindow.h

    include/Definitions.h
    include/leespoiler.h
    include/consolecmd.h
    include/massfbxultilities.h
    include/impcmd.h
    include/leetreemodel.h


    src/main.cpp
    src/mainwindow.cpp

    src/mainwindow.ui
    src/FbxOptions.ui
    src/Maintain.ui

    src/leespoiler.cpp
    LeeAutoExportFBX.rc
    src/massfbxultilities.cpp
    src/consolecmd.cpp
    src/impcmd.cpp
    src/leetreemodel.cpp
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

target_link_libraries(LeeAutoExportFBX PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::Network)

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

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/Scripts)

# Scripts Files
file(GLOB_RECURSE files "{CMAKE_CURRENT_SOURCE_DIR}/Scripts/" "*.py" "*.mel")

# Copy to build Folders
file(COPY ${files} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Scripts)


# if(NOT EXISTS(${LEEDEFINE}))
#     FILE(WRITE ${LEEDEFINE}
#     "#define MELEXPORTSCRIPT \"${MELEXPORT}\"\n"
#     "#define MASSFBXLOG \"${MASSFBXLOG}\"\n"
#     "#define BLENDEREXPORT \"${BLENDEREXPORT}\"\n"
#     "#define SCRIPTDIR \"${SCRIPT_DIR}/\"\n"
#     "#define MASSVERSION \"LeeMassFbx Tools  Version 1.0 \"\n"
#     )
#endif()
#copy file

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Scripts/MayaExportCmd.mel ${CMAKE_CURRENT_BINARY_DIR}/Scripts/MayaExportCmd.mel COPYONLY)
#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Scripts/BlenderExport.py ${CMAKE_CURRENT_BINARY_DIR}/Scripts/BlenderExport.py COPYONLY)


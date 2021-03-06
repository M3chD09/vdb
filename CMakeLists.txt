cmake_minimum_required(VERSION 3.5)

project(vdb VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS OpenGL REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)

set(PROJECT_SOURCES
    camera.cpp
    main.cpp
    glwidget.cpp
    Tool.cpp
    shaders.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(vdb
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET vdb APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(vdb SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(vdb
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(vdb PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(vdb PRIVATE Qt6::OpenGL)
target_link_libraries(vdb PRIVATE Qt6::OpenGLWidgets)

set_target_properties(vdb PROPERTIES
#    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(vdb)
endif()

install(
    DIRECTORY ${PROJECT_BINARY_DIR}/Release/
    DESTINATION /
)

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND windeployqt "$<TARGET_FILE:${PROJECT_NAME}>"
        --$<LOWER_CASE:$<CONFIG>> 
)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION /)
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
set(CPACK_NSIS_MENU_LINKS
    ${PROJECT_NAME}.exe ${PROJECT_NAME}
)
set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_VERSION_MAJOR "${vdb_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${vdb_VERSION_MINOR}")
include(CPack)

cmake_minimum_required(VERSION 3.5)

project(VRSA VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск GDAL
find_package(GDAL REQUIRED)
if(GDAL_FOUND)
    message(STATUS "GDAL found: ${GDAL_VERSION}")
else()
    # Альтернативный поиск через pkg-config
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(GDAL_PKG gdal)
        if(GDAL_PKG_FOUND)
            set(GDAL_INCLUDE_DIRS ${GDAL_PKG_INCLUDE_DIRS})
            set(GDAL_LIBRARIES ${GDAL_PKG_LIBRARIES})
            set(GDAL_FOUND TRUE)
        endif()
    endif()

    if(NOT GDAL_FOUND)
        find_path(GDAL_INCLUDE_DIRS gdal_priv.h PATHS /usr/include/gdal)
        find_library(GDAL_LIBRARIES NAMES gdal)
        if(GDAL_INCLUDE_DIRS AND GDAL_LIBRARIES)
            set(GDAL_FOUND TRUE)
        else()
            message(FATAL_ERROR "GDAL not found! Install with: sudo apt-get install libgdal-dev")
        endif()
    endif()
endif()


find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/customWidgets
    ${CMAKE_CURRENT_SOURCE_DIR}/gdal
    ${CMAKE_CURRENT_SOURCE_DIR}/vector
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#все файлы в папке customWidgets
file(GLOB CUSTOM_WIDGETS_SOURCES
    "customWidgets/*.cpp"
    "customWidgets/*.h"
    "customWidgets/*.ui"
)

file(GLOB GDAL_SOURCES
    "gdal/*.cpp"
    "gdal/*.h"
    "gdal/*.ui"
)

file(GLOB VECTOR_SOURCES
    "vector/*.cpp"
    "vector/*.h"
    "vector/*.ui"
)

file(GLOB COMMON_SOURCES
    "common/*.cpp"
    "common/*.h"
    "common/*.ui"
)

set(TS_FILES VRSA_ru_RU.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        test.ui
        ui/mainwindow.ui
        ${CUSTOM_WIDGETS_SOURCES}
        ${GDAL_SOURCES}
        ${VECTOR_SOURCES}
        ${COMMON_SOURCES}
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VRSA
        MANUAL_FINALIZATION
        icons.qrc
        ${PROJECT_SOURCES}
    )

    target_include_directories(VRSA PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/customWidgets
        ${CMAKE_CURRENT_SOURCE_DIR}/ui
        ${CMAKE_CURRENT_SOURCE_DIR}/gdal
        ${CMAKE_CURRENT_SOURCE_DIR}/vector
        ${CMAKE_CURRENT_SOURCE_DIR}/common
    )

    if(TARGET GDAL::GDAL)
        target_link_libraries(VRSA PRIVATE GDAL::GDAL)
    else()
        target_include_directories(VRSA PRIVATE ${GDAL_INCLUDE_DIRS})
        target_link_libraries(VRSA PRIVATE ${GDAL_LIBRARIES})
    endif()


    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(VRSA SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(VRSA
            ${PROJECT_SOURCES}
            icons.qrc
        )

        target_include_directories(VRSA PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/customWidgets
            ${CMAKE_CURRENT_SOURCE_DIR}/ui
            ${CMAKE_CURRENT_SOURCE_DIR}/gdal
            ${CMAKE_CURRENT_SOURCE_DIR}/vector
            ${CMAKE_CURRENT_SOURCE_DIR}/common
        )

        # === ДОБАВЬ ЭТО ДЛЯ GDAL (Qt5) ===
        if(TARGET GDAL::GDAL)
            target_link_libraries(VRSA PRIVATE GDAL::GDAL)
        else()
            target_include_directories(VRSA PRIVATE ${GDAL_INCLUDE_DIRS})
            target_link_libraries(VRSA PRIVATE ${GDAL_LIBRARIES})
        endif()
        # === КОНЕЦ ДОБАВЛЕНИЯ ===
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(VRSA PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(VRSA PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VRSA)
endif()

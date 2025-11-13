cmake_minimum_required(VERSION 3.5)
project(VRSA VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Указываем AUTOUIC где искать UI файлы
set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/customWidgets
)

option(BUILD_TESTS "Build tests" ON)

# ============================================================================
# ПОИСК QT И GDAL
# ============================================================================

find_package(Qt6 QUIET COMPONENTS Core Widgets LinguistTools)
if(Qt6_FOUND)
    message(STATUS "Qt6 found - using Qt6")
    set(QT_VERSION_MAJOR 6)
    set(QT_LIBS Qt6::Core Qt6::Widgets)
else()
    find_package(Qt5 QUIET COMPONENTS Core Widgets LinguistTools)
    if(Qt5_FOUND)
        message(STATUS "Qt5 found - using Qt5")
        set(QT_VERSION_MAJOR 5)
        set(QT_LIBS Qt5::Core Qt5::Widgets)
    else()
        message(FATAL_ERROR "Neither Qt6 nor Qt5 found!")
    endif()
endif()

find_package(GDAL REQUIRED)
message(STATUS "GDAL found: ${GDAL_VERSION}")

# ============================================================================
# БИБЛИОТЕКА vrsa_core
# ============================================================================

file(GLOB CORE_SOURCES
    "gdal/*.cpp"
    "vector/*.cpp"
    "common/*.cpp"
    "graphics/*.cpp"
    "services/*.cpp"
    "calculations/*.cpp"
    "georef/*.cpp"
)

# ЗАГОЛОВОЧНЫЕ ФАЙЛЫ для библиотеки
file(GLOB CORE_HEADERS
    "gdal/*.h"
    "vector/*.h"
    "common/*.h"
    "graphics/*.h"
    "services/*.h"
    "calculations/*.h"
    "georef/*.h"
)
add_library(vrsa_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(vrsa_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    gdal
    vector
    common
    graphics
    services
    calculations
    georef
)

target_link_libraries(vrsa_core PUBLIC
    GDAL::GDAL
    ${QT_LIBS}
)

# ============================================================================
# ГЛАВНОЕ ПРИЛОЖЕНИЕ VRSA
# ============================================================================

# ТОЛЬКО основные .cpp файлы
file(GLOB MAIN_SOURCES
    "main.cpp"
    "mainwindow.cpp"      # ← mainwindow.cpp в корне
)

file(GLOB CUSTOM_WIDGETS_CPP "customWidgets/*.cpp")
file(GLOB UI_CPP "ui/*.cpp")

# Заголовочные файлы для приложения
file(GLOB MAIN_HEADERS
    "mainwindow.h"
    "*.h"  # другие заголовки в корне
)

file(GLOB CUSTOM_WIDGETS_HEADERS "customWidgets/*.h")
file(GLOB UI_HEADERS "ui/*.h")

# ВСЕ UI файлы включая поддиректории
file(GLOB_RECURSE UI_FILES
    "*.ui"                # ui файлы в корне
    "customWidgets/*.ui"  # ui файлы в customWidgets
    "ui/*.ui"             # ui файлы в ui/
)

set(TS_FILES VRSA_ru_RU.ts)

if(QT_VERSION_MAJOR EQUAL 6)
    qt6_add_executable(VRSA
        ${MAIN_SOURCES}
        ${MAIN_HEADERS}
        ${CUSTOM_WIDGETS_CPP}
        ${CUSTOM_WIDGETS_HEADERS}
        ${UI_CPP}
        ${UI_HEADERS}
        ${UI_FILES}        # ← ВСЕ ui файлы
        icons.qrc
    )

    target_link_libraries(VRSA PRIVATE
        vrsa_core
        Qt6::Widgets
    )

    # ВКЛЮЧАЕМ все нужные директории
    target_include_directories(VRSA PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        customWidgets
        ui
        ${CMAKE_CURRENT_BINARY_DIR}  # ← для сгенерированных ui_*.h файлов
    )

    qt6_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

else()
    qt5_wrap_ui(UI_HEADERS ${UI_FILES})

    add_executable(VRSA
        ${MAIN_SOURCES}
        ${MAIN_HEADERS}
        ${CUSTOM_WIDGETS_CPP}
        ${CUSTOM_WIDGETS_HEADERS}
        ${UI_CPP}
        ${UI_HEADERS}
        ${UI_GENERATED_HEADERS}
        ${UI_FILES}

        icons.qrc
    )

    target_link_libraries(VRSA PRIVATE
        vrsa_core
        Qt5::Widgets
    )

    target_include_directories(VRSA PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        customWidgets
        ui
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    qt5_add_translation(QM_FILES ${TS_FILES})
endif()

set_target_properties(VRSA PROPERTIES WIN32_EXECUTABLE TRUE)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VRSA)
endif()

# ============================================================================
# ТЕСТЫ
# ============================================================================

if(BUILD_TESTS)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tests/data)
    #file(COPY tests/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/tests/data)

    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(test_vector_feature tests/unit/test_vector_feature.cpp
         tests/unit/test_vector_reader.cpp)

    target_link_libraries(test_vector_feature PRIVATE
        vrsa_core
        GTest::gtest_main
    )

    target_include_directories(test_vector_feature PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )

    include(GoogleTest)
    gtest_discover_tests(test_vector_feature)

    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS test_vector_feature
        COMMENT "Running tests"
    )
endif()

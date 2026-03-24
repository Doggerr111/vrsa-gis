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

# ============================================================================
# ОПЦИИ СБОРКИ
# ============================================================================
option(BUILD_TESTS "Build tests" ON)
option(VRSA_ENABLE_TEST_UTILS "Include test utilities in core library" ON)



# ============================================================================
# ПОИСК QT, GDAL и GEOS
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

#GEOS

message(STATUS "Looking for GEOS...")

# GEOS через pkg-config с извлечением приватных библиотек
find_package(PkgConfig REQUIRED)

# Находим geos (C API)
pkg_check_modules(GEOS REQUIRED geos IMPORTED_TARGET)

# Извлекаем приватные библиотеки (там лежит libgeos)
if(GEOS_LIBRARIES)
    # Добавляем приватные библиотеки к публичным
    # В Libs.private: -lgeos -lstdc++ -lm
    set(GEOS_CPP_LIBRARIES ${GEOS_LIBRARIES})

    # Добавляем -lgeos из приватных если его нет в публичных
    list(APPEND GEOS_CPP_LIBRARIES geos)
endif()





if(GEOS_FOUND)
    message(STATUS "GEOS version: ${GEOS_VERSION}")
    message(STATUS "GEOS include dirs: ${GEOS_INCLUDEDIR}")
    message(STATUS "GEOS libraries: ${GEOS_CPP_LIBRARIES}")
    message(STATUS "GEOS library dirs: ${GEOS_LIBRARY_DIRS}")

    # Показать полные флаги
    message(STATUS "GEOS cflags: ${GEOS_CFLAGS}")
    message(STATUS "GEOS ldflags: ${GEOS_LDFLAGS}")
endif()

# Показать все переменные, связанные с GEOS
get_cmake_property(_variableNames VARIABLES)
list(SORT _variableNames)
foreach(_variableName ${_variableNames})
    if(_variableName MATCHES "GEOS")
        message(STATUS "  ${_variableName}=${${_variableName}}")
    endif()
endforeach()



# ============================================================================
# БИБЛИОТЕКА vrsa_core
# ============================================================================

file(GLOB CORE_SOURCES
    "gdal/*.cpp"
    "vector/*.cpp"
    "common/*.cpp"
    "graphics/*.cpp"
    "graphics/symbols/*.cpp"
    "graphics/drawing_policies/*.cpp"
    "calculations/*.cpp"
    "georef/*.cpp"
    "raster/*.cpp"
    "tools/*.cpp"
    "test_utils/*.cpp"
    "geometry/*.cpp"
    "vectorlayertools/*.cpp"
    "vectorlayertools/spatial_operations/*.cpp"
    "spatialref/*.cpp"
    "rastertools/*.cpp"
    )
file(GLOB_RECURSE GRAPHICS_SOURCES "graphics/*.cpp")
file(GLOB_RECURSE GRAPHICS_HEADERS "graphics/*.h")

# ЗАГОЛОВОЧНЫЕ ФАЙЛЫ для библиотеки
file(GLOB CORE_HEADERS
    "gdal/*.h"
    "vector/*.h"
    "common/*.h"
    "graphics/*.h"
    "graphics/symbols/*.h"
    "graphics/drawing_policies/*.h"
    "calculations/*.h"
    "georef/*.h"
    "raster/*.h"
    "tools/*.h"
    "test_utils/*.h"
    "geometry/*.h"
    "vectorlayertools/*.h"
    "vectorlayertools/spatial_operations/*.h"
    "spatialref/*.h"
    "rastertools/*.h"
    )
add_library(vrsa_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
#добавляем тестовые функции в ядре
target_compile_definitions(vrsa_core PUBLIC VRSA_ENABLE_TEST_UTILS=$<BOOL:${VRSA_ENABLE_TEST_UTILS}>)
target_include_directories(vrsa_core SYSTEM PRIVATE ${GEOS_INCLUDEDIR})
target_include_directories(vrsa_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    gdal
    vector
    common
    graphics
    graphics/symbols
    graphics/drawing_policies
    calculations
    georef
    raster
    tools
    test_utils
    geometry
    vectorlayertools
    vectorlayertools/spatial_operations
    spatialref
    rastertools

    )


target_link_libraries(vrsa_core PUBLIC
    GDAL::GDAL
    ${GEOS_STATIC_LIBRARIES}
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

file(GLOB CUSTOM_WIDGETS_CPP
    "customWidgets/*.cpp"
    "customWidgets/stylingWidgets/*.cpp"
    "customWidgets/rasterProccessingWidgets/*.cpp"
    "services/*.cpp"
    )
file(GLOB UI_CPP "ui/*.cpp")

# Заголовочные файлы для приложения
file(GLOB MAIN_HEADERS
    "mainwindow.h"
    "*.h"  # другие заголовки в корне
    )

file(GLOB CUSTOM_WIDGETS_HEADERS
    "customWidgets/*.h"
    "customWidgets/stylingWidgets/*.h"
    "customWidgets/rasterProccessingWidgets/*.h"
    "services/*.h"
    )
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
        appStyles.qrc
        )

    target_link_libraries(VRSA PRIVATE
        vrsa_core
        Qt6::Widgets
        )

    # ВКЛЮЧАЕМ все нужные директории
    target_include_directories(VRSA PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        customWidgets
        customWidgets/stylingWidgets
        customWidgets/rasterProccessingWidgets
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
        appStyles.qrc
        )

    target_link_libraries(VRSA PRIVATE
        vrsa_core
        Qt5::Widgets
        )

    target_include_directories(VRSA PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        customWidgets
        customWidgets/stylingWidgets
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

    add_executable(test_geos
        tests/unit/test_geos_converter_via_wkt.cpp
        tests/unit/test_geos_converter_via_coords.cpp
        )

    target_link_libraries(test_geos PRIVATE
        vrsa_core
        GTest::gtest_main
        )

    add_executable(test_gdal_wrapper
        tests/unit/test_vector_reader.cpp
        tests/unit/test_vector_creator.cpp
        tests/unit/test_vector_layer.cpp
        tests/unit/test_vector_feature.cpp
        tests/unit/test_spatial_ref.cpp
        tests/unit/test_coordinate_transformer.cpp
        )

    target_link_libraries(test_gdal_wrapper PRIVATE
        vrsa_core
        GTest::gtest_main
        )

    add_executable(test_raster
        tests/unit/test_raster.cpp)

    target_link_libraries(test_raster PRIVATE
        vrsa_core
        GTest::gtest_main
        )

    add_executable(test_graphics
        tests/unit/test_feature_drawing_policy.cpp)

    target_link_libraries(test_graphics PRIVATE
        vrsa_core
        GTest::gtest_main
        )

    target_include_directories(test_geos PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        )

    target_include_directories(test_gdal_wrapper PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        )

    target_include_directories(test_raster PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        )

    target_include_directories(test_graphics PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        )

    include(GoogleTest)
    gtest_discover_tests(test_geos)
    gtest_discover_tests(test_gdal_wrapper)
    gtest_discover_tests(test_raster)
    gtest_discover_tests(test_graphics)

    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS test_vector_feature
        COMMENT "Running tests"
        )
endif()

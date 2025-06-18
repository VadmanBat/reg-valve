cmake_minimum_required(VERSION 3.28)
project(RegValve VERSION 1.0 LANGUAGES CXX)

# ========== Настройка C++ ==========
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(STATIC_BUILD OFF)

# ========== Настройка Qt ==========
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(Qt6_DIR "C:/Qt/Qt6.8.0/mingw_64/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "C:/Qt/Qt6.8.0/mingw_64")
find_package(Qt6 COMPONENTS Widgets Charts REQUIRED)

# ========== Источники и исполняемый файл ==========
include(cmake/source-files.cmake)
add_executable(${PROJECT_NAME} ${SOURCES})

# ========== Настройки компиляции ==========
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Widgets
        Qt6::Charts
)
include(cmake/compiler-flags.cmake)

# ========== Оптимизация сборки ==========
if (CCACHE_FOUND)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_COMPILER_LAUNCHER ccache)
endif()
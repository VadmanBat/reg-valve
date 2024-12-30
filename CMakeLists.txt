cmake_minimum_required(VERSION 3.28)
set(PROJECT_NAME RegValve)
project(${PROJECT_NAME})

set(Qt6_DIR "C:/Qt/Qt6.8.0/mingw_64/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "C:/Qt/Qt6.8.0/mingw_64")

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets Charts REQUIRED)

file(GLOB_RECURSE HEADERS "code/*.h" "code/*.hpp" "code/*.hxx")
file(GLOB_RECURSE SOURCES "code/*.cpp")
add_executable(${PROJECT_NAME} main.cpp ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Charts)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -funroll-loops -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
#    target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-lto>)
#    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-DNDEBUG>)
#    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-s>)
#    set_target_properties(${PROJECT_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
    set_target_properties(${PROJECT_NAME} PROPERTIES QT_CONFIG release)
endif()

if (DEFINED ENV{CCACHE})
    set_property(TARGET ${PROJECT_NAME} PROPERTY C_COMPILER_LAUNCHER "ccache")
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_COMPILER_LAUNCHER "ccache")
endif()
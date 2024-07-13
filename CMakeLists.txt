cmake_minimum_required(VERSION 3.28)
set(PROJECT_NAME RegValve)
project(${PROJECT_NAME})

set(Qt5_DIR "C:/Qt/Qt5.12.12/5.12.12/mingw73_64/lib/cmake/Qt5")

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -march=native -funroll-loops")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Charts REQUIRED)

file(GLOB_RECURSE HEADERS "code/*.h" "code/*.hpp")
file(GLOB_RECURSE SOURCES "code/*.cpp")
add_executable(${PROJECT_NAME} main.cpp ${HEADERS} ${SOURCES}
        code/qt-application/tabs/num-tab.cpp
        code/qt-application/tabs/static-funcs.cpp)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Charts)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()
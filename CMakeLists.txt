cmake_minimum_required(VERSION 3.28)

# Before project()/Qt AUTOMOC: fix MSYS2 moc DLL search (0xC0000135 without ucrt64/bin).
include("${CMAKE_CURRENT_LIST_DIR}/cmake/msys-qt-env.cmake")

project(RegValve VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(STATIC_BUILD OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS
        ${CMAKE_SOURCE_DIR}/ui
        ${CMAKE_SOURCE_DIR}/ui/tabs
        ${CMAKE_SOURCE_DIR}/ui/dialogs
)

# Qt: override with -DCMAKE_PREFIX_PATH / -DQt6_DIR, else try common installs
if (NOT Qt6_DIR)
    if (EXISTS "C:/msys64/ucrt64/lib/cmake/Qt6/Qt6Config.cmake")
        set(Qt6_DIR "C:/msys64/ucrt64/lib/cmake/Qt6" CACHE PATH "Qt6 CMake package dir")
        list(PREPEND CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
    elseif (EXISTS "C:/Qt/Qt6.8.0/mingw_64/lib/cmake/Qt6/Qt6Config.cmake")
        set(Qt6_DIR "C:/Qt/Qt6.8.0/mingw_64/lib/cmake/Qt6" CACHE PATH "Qt6 CMake package dir")
        list(PREPEND CMAKE_PREFIX_PATH "C:/Qt/Qt6.8.0/mingw_64")
    endif ()
endif ()
find_package(Qt6 COMPONENTS Widgets Charts REQUIRED)

# numina static library
set(NUMINA_ROOT "C:/cpp/projects/static-libs/numina" CACHE PATH "Path to numina sources")
if (NOT EXISTS "${NUMINA_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "numina not found at NUMINA_ROOT=${NUMINA_ROOT}")
endif ()
add_subdirectory("${NUMINA_ROOT}" "${CMAKE_BINARY_DIR}/numina_build")

include(cmake/source-files.cmake)
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Widgets
        Qt6::Charts
        numina::numina
)

# Runtime assets next to the executable (cwd when running from CLion is often the build dir).
# MainWindow looks for: data/styles/…, data/fonts/… and fallback styles/…
set(REGVALVE_DATA_DIR "${CMAKE_SOURCE_DIR}/data")
if (EXISTS "${REGVALVE_DATA_DIR}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/styles"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${REGVALVE_DATA_DIR}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${REGVALVE_DATA_DIR}/styles"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/styles"
            COMMENT "Copying data/ (styles, fonts) next to ${PROJECT_NAME}"
            VERBATIM
    )
endif ()

include(cmake/compiler-flags.cmake)

if (CCACHE_FOUND)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_COMPILER_LAUNCHER ccache)
endif ()

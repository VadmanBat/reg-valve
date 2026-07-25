# MSYS2 UCRT64: moc/uic/rcc live under share/qt6/bin and need ucrt64/bin on PATH
# (Windows exit 0xC0000135 = STATUS_DLL_NOT_FOUND). g++ -E for AUTOMOC predefs
# also needs the same bin dir (cc1plus etc.).

set(_MSYS_UCRT_ROOT "C:/msys64/ucrt64")
set(_MSYS_UCRT_BIN "${_MSYS_UCRT_ROOT}/bin")

if (EXISTS "${_MSYS_UCRT_BIN}")
    # Configure-time subprocesses (find_package, try_compile, AUTOMOC test run)
    set(ENV{PATH} "${_MSYS_UCRT_BIN};$ENV{PATH}")

    set(_WRAP_DIR "${CMAKE_BINARY_DIR}/qt-tool-wrappers")
    file(MAKE_DIRECTORY "${_WRAP_DIR}")

    # Launcher for compile/link/custom: inject PATH for Ninja rules under CLion
    set(_PATH_LAUNCHER "${_WRAP_DIR}/path-env.cmd")
    file(WRITE "${_PATH_LAUNCHER}"
            "@echo off\r\n"
            "set \"PATH=${_MSYS_UCRT_BIN};%PATH%\"\r\n"
            "%*\r\n"
    )
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${_PATH_LAUNCHER}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${_PATH_LAUNCHER}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_CUSTOM "${_PATH_LAUNCHER}")

    set(_MOC_CANDIDATES
            "${_MSYS_UCRT_ROOT}/share/qt6/bin/moc.exe"
            "${_MSYS_UCRT_BIN}/moc.exe"
            "${_MSYS_UCRT_BIN}/moc-qt6.exe"
    )
    set(_UIC_CANDIDATES
            "${_MSYS_UCRT_ROOT}/share/qt6/bin/uic.exe"
            "${_MSYS_UCRT_BIN}/uic.exe"
            "${_MSYS_UCRT_BIN}/uic-qt6.exe"
    )
    set(_RCC_CANDIDATES
            "${_MSYS_UCRT_ROOT}/share/qt6/bin/rcc.exe"
            "${_MSYS_UCRT_BIN}/rcc.exe"
            "${_MSYS_UCRT_BIN}/rcc-qt6.exe"
    )

    foreach (_tool IN ITEMS moc uic rcc)
        string(TOUPPER "${_tool}" _TOOL_UP)
        set(_found "")
        foreach (_cand IN LISTS _${_TOOL_UP}_CANDIDATES)
            if (EXISTS "${_cand}")
                set(_found "${_cand}")
                break()
            endif ()
        endforeach ()
        if (_found)
            set(_wrap "${_WRAP_DIR}/${_tool}.cmd")
            file(WRITE "${_wrap}"
                    "@echo off\r\n"
                    "set \"PATH=${_MSYS_UCRT_BIN};%PATH%\"\r\n"
                    "\"${_found}\" %*\r\n"
            )
            set(CMAKE_AUTO${_TOOL_UP}_EXECUTABLE "${_wrap}" CACHE FILEPATH "Wrapped Qt ${_tool}" FORCE)
            message(STATUS "Qt ${_tool}: ${_found} (via ${_wrap})")
        endif ()
    endforeach ()
endif ()

set(UI_FILES
        ${CMAKE_SOURCE_DIR}/ui/mainwindow.ui
        ${CMAKE_SOURCE_DIR}/ui/tabs/id-tab.ui
        ${CMAKE_SOURCE_DIR}/ui/tabs/analysis-tab.ui
        ${CMAKE_SOURCE_DIR}/ui/tabs/synthesis-tab.ui
        ${CMAKE_SOURCE_DIR}/ui/tabs/rim-tab.ui
        ${CMAKE_SOURCE_DIR}/ui/dialogs/mod-par-dialog.ui
        ${CMAKE_SOURCE_DIR}/ui/dialogs/id-settings-dialog.ui
        ${CMAKE_SOURCE_DIR}/ui/dialogs/help-dialog.ui
        ${CMAKE_SOURCE_DIR}/ui/dialogs/chart-dialog.ui
        ${CMAKE_SOURCE_DIR}/ui/dialogs/tran-func-dialog.ui
)

set(SOURCES
        ${CMAKE_SOURCE_DIR}/main.cpp

        ${CMAKE_SOURCE_DIR}/code/app/mainwindow.cpp
        ${CMAKE_SOURCE_DIR}/code/tabs/id-tab.cpp
        ${CMAKE_SOURCE_DIR}/code/tabs/analysis-tab.cpp
        ${CMAKE_SOURCE_DIR}/code/tabs/synthesis-tab.cpp
        ${CMAKE_SOURCE_DIR}/code/tabs/rim-tab.cpp

        ${CMAKE_SOURCE_DIR}/code/dialogs/mod-par-dialog.cpp
        ${CMAKE_SOURCE_DIR}/code/dialogs/id-settings-dialog.cpp
        ${CMAKE_SOURCE_DIR}/code/dialogs/help-dialog.cpp
        ${CMAKE_SOURCE_DIR}/code/dialogs/chart-dialog.cpp
        ${CMAKE_SOURCE_DIR}/code/dialogs/tran-func-dialog.cpp

        ${CMAKE_SOURCE_DIR}/code/widgets/double-slider.cpp
        ${CMAKE_SOURCE_DIR}/code/widgets/reg-parameter.cpp
        ${CMAKE_SOURCE_DIR}/code/widgets/regulation-widget.cpp
        ${CMAKE_SOURCE_DIR}/code/widgets/tran-func-form.cpp
        ${CMAKE_SOURCE_DIR}/code/widgets/tf-display-widget.cpp

        ${CMAKE_SOURCE_DIR}/code/charts/chart-utils.cpp
        ${CMAKE_SOURCE_DIR}/code/charts/chart-panel.cpp
        ${CMAKE_SOURCE_DIR}/code/charts/response-chart-bank.cpp

        ${UI_FILES}
)

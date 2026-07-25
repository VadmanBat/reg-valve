set(UI_FILES
        ${CMAKE_SOURCE_DIR}/ui/mainwindow.ui
        ${CMAKE_SOURCE_DIR}/ui/tabs/exp_tab.ui
        ${CMAKE_SOURCE_DIR}/ui/tabs/num_tab.ui
        ${CMAKE_SOURCE_DIR}/ui/tabs/reg_tab.ui
        ${CMAKE_SOURCE_DIR}/ui/dialogs/mod_par_dialog.ui
        ${CMAKE_SOURCE_DIR}/ui/dialogs/help_dialog.ui
        ${CMAKE_SOURCE_DIR}/ui/dialogs/chart_dialog.ui
        ${CMAKE_SOURCE_DIR}/ui/dialogs/tran_func_dialog.ui
)

set(SOURCES
        ${CMAKE_SOURCE_DIR}/main.cpp

        ${CMAKE_SOURCE_DIR}/code/app/mainwindow.cpp
        ${CMAKE_SOURCE_DIR}/code/tabs/exp_tab.cpp
        ${CMAKE_SOURCE_DIR}/code/tabs/num_tab.cpp
        ${CMAKE_SOURCE_DIR}/code/tabs/reg_tab.cpp

        ${CMAKE_SOURCE_DIR}/code/dialogs/mod_par_dialog.cpp
        ${CMAKE_SOURCE_DIR}/code/dialogs/help_dialog.cpp
        ${CMAKE_SOURCE_DIR}/code/dialogs/chart_dialog.cpp
        ${CMAKE_SOURCE_DIR}/code/dialogs/tran_func_dialog.cpp

        ${CMAKE_SOURCE_DIR}/code/widgets/double_slider.cpp
        ${CMAKE_SOURCE_DIR}/code/widgets/reg_parameter.cpp
        ${CMAKE_SOURCE_DIR}/code/widgets/regulation_widget.cpp
        ${CMAKE_SOURCE_DIR}/code/widgets/tran_func_form.cpp

        ${CMAKE_SOURCE_DIR}/code/charts/chart_utils.cpp

        ${UI_FILES}
)

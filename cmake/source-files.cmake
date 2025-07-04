set(CORE_SOURCES
        ../main.cpp
        ../code/math-core.hpp
        ../code/convert-core.hpp
        ../code/style-core.hpp
)

set(NUMINA_SOURCES
        ../code/numina/constants/constants.hpp
        ../code/numina/fraction/invlaplace.hpp
        ../code/numina/fraction/parfrac.hpp
        ../code/numina/matrix/solve.hpp
        ../code/numina/polynomial/deflate.hpp
        ../code/numina/polynomial/multiply.hpp
        ../code/numina/polynomial/solve.hpp
        ../code/numina/terms/term.hpp
        ../code/numina/terms/term-expression.hpp
        ../code/numina/terms/time-terms/time-terms.hpp
        ../code/numina/terms/time-terms/utime-terms.hpp
)

set(QT_APP_SOURCES
        ../code/qt-application/application.h
        ../code/qt-application/static-functions/model-param.cpp
        ../code/qt-application/static-functions/charts.cpp
        ../code/qt-application/app-functions/constructor.cpp
        ../code/qt-application/tabs/exp-tab.cpp
        ../code/qt-application/static-functions/general.cpp
        ../code/qt-application/button-functions/reg-functions.cpp
        ../code/qt-application/tabs/reg-tab.cpp
        ../code/qt-application/button-functions/num-functions.cpp
        ../code/qt-application/tabs/num-tab.cpp
        ../code/qt-application/static-functions/styles.cpp
        ../code/qt-application/button-functions/exp-functions.cpp
)

set(SERIES_SOURCES
        ../code/series/complex-series.hpp
        ../code/series/set-series.hpp
        ../code/series/series.hpp
)

set(STRUCTURES_SOURCES
        ../code/structures/reg-parameter.hpp
        ../code/structures/model-param.hpp
        ../code/structures/dialogs/chart-dialog.hpp
        ../code/structures/double-slider.hpp
        ../code/structures/reg-widget.hpp
        ../code/structures/forms/tran-func-form.hpp
        ../code/structures/dialogs/help-dialog.hpp
        ../code/structures/dialogs/mod-par-dialog.hpp
        ../code/structures/dialogs/tran-func-dialog.hpp
)

set(TRANSFER_FUNCTION_SOURCES
        ../code/transfer-function/transfer-function.hpp
        ../code/transfer-function/operators/assign.cpp
        ../code/transfer-function/compute-responses/impulse-response.cpp
        ../code/transfer-function/compute-responses/transient-response.cpp
        ../code/transfer-function/compute-algorithms/error-criterion.cpp
        ../code/transfer-function/compute-algorithms/freq-searche.cpp
        ../code/transfer-function/compute-responses/frequency-response.cpp
        ../code/transfer-function/constructors.cpp
)

set(SOURCES
        ${CORE_SOURCES}
        ${NUMINA_SOURCES}
        ${QT_APP_SOURCES}
        ${SERIES_SOURCES}
        ${STRUCTURES_SOURCES}
        ${TRANSFER_FUNCTION_SOURCES}
)
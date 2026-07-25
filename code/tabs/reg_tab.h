#pragma once

#include "code/model/model_param.hpp"
#include "code/series/complex_series.hpp"
#include "code/series/series.hpp"
#include "code/series/set_series.hpp"
#include "code/widgets/reg_parameter.h"
#include "code/widgets/regulation_widget.h"
#include "code/widgets/tran_func_form.h"

#include "numina/classes/control/transfer-function.h"

#include <QChart>
#include <QTimer>
#include <QWidget>

#include <vector>

namespace Ui {
class RegTab;
}

class RegTab : public QWidget {
    Q_OBJECT
public:
    explicit RegTab(QWidget* parent = nullptr);
    ~RegTab() override;

private slots:
    void addTransferFunction();
    void replaceTransferFunction();
    void clearCharts();
    void openSettings();
    void openHelp();
    void scheduleReplace();

private:
    void installCustomWidgets();
    void setupMetrics();
    void showError(const QString& message);
    /// Recompute closed-loop TF and push/update charts. replaceLast → in-place last series.
    void applyCurrentRegulator(bool replaceLast);

    Ui::RegTab* ui;
    TranFuncForm* form_{nullptr};
    RegulationWidget* metrics_{nullptr};
    std::vector<RegParameter*> parameters_;
    QChart* chartTran_{nullptr};
    QChart* chartFreq_{nullptr};
    SetSeries<Series> tranSeries_;
    SetSeries<ComplexSeries> freqSeries_;
    ModelParam modelParam_;
    numina::TransferFunction currentTf_;
    std::size_t seriesIndex_{0};
    QTimer debounce_; ///< Coalesces slider events to ~1 update per display frame.
    bool hasSeries_{false};
};

#pragma once

#include "code/model/model_param.hpp"
#include "code/series/complex_series.hpp"
#include "code/series/series.hpp"
#include "code/series/set_series.hpp"
#include "code/widgets/regulation_widget.h"
#include "code/widgets/tran_func_form.h"

#include "numina/classes/control/transfer-function.h"

#include <QChart>
#include <QWidget>

namespace Ui {
class NumTab;
}

class NumTab : public QWidget {
    Q_OBJECT
public:
    explicit NumTab(QWidget* parent = nullptr);
    ~NumTab() override;

private slots:
    void addTransferFunction();
    void replaceTransferFunction();
    void clearCharts();
    void openSettings();

private:
    void showError(const QString& message);
    void setupMetrics();
    void installCustomWidgets();

    Ui::NumTab* ui;
    TranFuncForm* form_{nullptr};
    RegulationWidget* metrics_{nullptr};
    QChart* chartTran_{nullptr};
    QChart* chartFreq_{nullptr};
    SetSeries<Series> tranSeries_;
    SetSeries<ComplexSeries> freqSeries_;
    ModelParam modelParam_;
    numina::TransferFunction currentTf_;
    std::size_t seriesIndex_{0};
};

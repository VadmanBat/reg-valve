#pragma once

#include "code/charts/response_chart_bank.h"
#include "code/model/model_param.hpp"
#include "code/widgets/regulation_widget.h"
#include "code/widgets/tran_func_form.h"

#include "numina/classes/control/transfer-function.h"

#include <QWidget>

class QMenu;

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
    void openChartsMenu();

private:
    void showError(const QString& message);
    void setupMetrics();
    void installCustomWidgets();

    Ui::NumTab* ui;
    TranFuncForm* form_{nullptr};
    RegulationWidget* metrics_{nullptr};
    ResponseChartBank* charts_{nullptr};
    QMenu* chartsMenu_{nullptr};
    ModelParam modelParam_;
    numina::TransferFunction currentTf_;
};

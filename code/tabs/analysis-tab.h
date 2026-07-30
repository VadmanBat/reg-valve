#pragma once

#include "code/charts/response-chart-bank.h"
#include "code/model/model-param.hpp"
#include "code/widgets/regulation-widget.h"
#include "code/widgets/tran-func-form.h"

#include "numina/classes/control/transfer-function.h"

#include <QWidget>

class QMenu;

namespace Ui {
class AnalysisTab;
}

class AnalysisTab : public QWidget {
    Q_OBJECT
public:
    explicit AnalysisTab(QWidget* parent = nullptr);
    ~AnalysisTab() override;

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

    Ui::AnalysisTab* ui;
    TranFuncForm* form_{nullptr};
    RegulationWidget* metrics_{nullptr};
    ResponseChartBank* charts_{nullptr};
    QMenu* chartsMenu_{nullptr};
    ModelParam modelParam_;
    numina::TransferFunction currentTf_;
};

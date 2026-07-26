#pragma once

#include "code/charts/response_chart_bank.h"
#include "code/model/model_param.hpp"
#include "code/widgets/reg_parameter.h"
#include "code/widgets/regulation_widget.h"
#include "code/widgets/tran_func_form.h"

#include "numina/classes/control/transfer-function.h"

#include <QWidget>

#include <vector>

class QMenu;

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

private:
    void installCustomWidgets();
    void setupMetrics();
    void showError(const QString& message);
    void applyCurrentRegulator(bool replaceLast);

    Ui::RegTab* ui;
    TranFuncForm* form_{nullptr};
    RegulationWidget* metrics_{nullptr};
    std::vector<RegParameter*> parameters_;
    ResponseChartBank* charts_{nullptr};
    QMenu* chartsMenu_{nullptr};
    ModelParam modelParam_;
    numina::TransferFunction currentTf_;
};

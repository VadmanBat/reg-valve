#pragma once

#include "code/charts/response-chart-bank.h"
#include "code/model/model-param.hpp"
#include "code/widgets/reg-parameter.h"
#include "code/widgets/regulation-widget.h"
#include "code/widgets/tf-form/tran-func-form.h"
#include "numina/classes/control/transfer-function.h"

#include <QWidget>
#include <vector>

class QMenu;

namespace Ui {
class SynthesisTab;
}

class SynthesisTab : public QWidget {
    Q_OBJECT

private:
    Ui::SynthesisTab* ui;
    TranFuncForm* form_{nullptr};
    RegulationWidget* metrics_{nullptr};
    std::vector<RegParameter*> parameters_;
    ResponseChartBank* charts_{nullptr};
    QMenu* charts_menu_{nullptr};
    ModelParam model_param_;
    numina::TransferFunction current_tf_;

    void install_custom_widgets();
    void setup_metrics();
    void show_error(const QString& message);
    void apply_current_regulator(bool replace_last);

private slots:
    void addTransferFunction();
    void replaceTransferFunction();
    void clearCharts();
    void openSettings();
    void openHelp();
    void autoSynthesize();

public:
    explicit SynthesisTab(QWidget* parent = nullptr);
    ~SynthesisTab() override;
};

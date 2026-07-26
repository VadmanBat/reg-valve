#pragma once

#include "code/charts/response_chart_bank.h"
#include "code/model/model_param.hpp"
#include "code/widgets/regulation_widget.h"
#include "code/widgets/tf_display_widget.h"

#include "numina/classes/control/transfer-function.h"

#include <QString>
#include <QWidget>

#include <utility>
#include <vector>

class QMenu;

namespace Ui {
class ExpTab;
}

class ExpTab : public QWidget {
    Q_OBJECT
public:
    explicit ExpTab(QWidget* parent = nullptr);
    ~ExpTab() override;

private slots:
    void openFile();
    void runIdentification();
    void clearAll();
    void openSettings();

private:
    enum class Method : int {
        StepResponse = 0,
        ValveSignal  = 1,
    };

    void installCustomWidgets();
    void setupMetrics();
    void showError(const QString& message);
    void showInfo(const QString& message);
    [[nodiscard]] bool loadStepFile(const QString& path);
    [[nodiscard]] bool loadValveSignalFile(const QString& path);
    void applyIdentificationResult(const numina::TransferFunction& tf,
                                   const std::vector<std::pair<double, double>>& experimentalH);

    Ui::ExpTab* ui;
    TfDisplayWidget* display_{nullptr};
    RegulationWidget* metrics_{nullptr};
    ResponseChartBank* charts_{nullptr};
    QMenu* chartsMenu_{nullptr};
    ModelParam modelParam_;
    numina::TransferFunction identifiedTf_;

    QString filePath_;
    std::vector<std::pair<double, double>> stepSeries_;   ///< loaded h(t) or recovered h(t)
    std::vector<std::pair<double, double>> valveSeries_;  ///< u(t) for valve/signal mode
    std::vector<std::pair<double, double>> signalSeries_; ///< y(t) for valve/signal mode
    bool hasData_{false};
};

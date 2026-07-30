#pragma once

#include "code/charts/response-chart-bank.h"
#include "code/model/id-settings.hpp"
#include "code/model/model-param.hpp"
#include "code/widgets/regulation-widget.h"
#include "code/widgets/tf-display-widget.h"

#include "numina/classes/control/transfer-function.h"

#include <QString>
#include <QWidget>

#include <utility>
#include <vector>

class QMenu;

namespace Ui {
class IdTab;
}

/// Идентификация: файл → Simoyu / Дюамель+Simoyu (+τ) → ПФ и графики.
class IdTab : public QWidget {
    Q_OBJECT
public:
    explicit IdTab(QWidget* parent = nullptr);
    ~IdTab() override;

private slots:
    void openFile();
    void runIdentification();
    void clearAll();
    void openSettings();
    void openIdSettings();

private:
    enum class Method : int {
        StepResponse = 0,
        ValveSignal  = 1,
    };

    using Series = std::vector<std::pair<double, double>>;

    Ui::IdTab* ui;
    TfDisplayWidget* display_{nullptr};
    RegulationWidget* metrics_{nullptr};
    ResponseChartBank* charts_{nullptr};
    QMenu* chartsMenu_{nullptr};
    ModelParam modelParam_;
    IdSettings idSettings_;
    numina::TransferFunction identifiedTf_;
    double identifiedTau_{0.0};

    QString filePath_;
    Series stepSeries_;
    Series valveSeries_;
    Series signalSeries_;
    bool hasData_{false};

    void install_custom_widgets();
    void setup_metrics();
    void show_error(const QString& message);
    [[nodiscard]] bool load_step_file(const QString& path);
    [[nodiscard]] bool load_valve_signal_file(const QString& path);
    void apply_result(const numina::TransferFunction& plant, double tau, const Series& experimental_h);
};

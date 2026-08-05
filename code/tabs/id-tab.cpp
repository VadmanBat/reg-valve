#include "code/tabs/id-tab.h"

#include "code/dialogs/id-settings-dialog.h"
#include "code/dialogs/mod-par-dialog.h"
#include "code/tabs/tab-charts-button.hpp"
#include "ui_id-tab.h"

#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

IdTab::IdTab(QWidget* parent) : QWidget(parent), ui(new Ui::IdTab) {
    ui->setupUi(this);
    install_custom_widgets();
    setup_metrics();

    charts_ = new ResponseChartBank(this);
    ui->chartsLayout->addWidget(charts_);
    ui->verticalLayout->setStretch(0, 0);
    ui->verticalLayout->setStretch(1, 0);
    ui->verticalLayout->setStretch(2, 1);

    ChartVisibility vis;
    vis.transient = true;
    vis.nyquist   = true;
    vis.impulse = vis.amplitude = vis.phase = false;
    charts_->setVisibility(vis);

    charts_menu_     = new QMenu(this);
    auto* charts_btn = tab_ui::makeChartsButton(this, charts_, charts_menu_);
    ui->buttonLayout->insertWidget(ui->buttonLayout->indexOf(ui->idSettingsButton), charts_btn);

    ui->methodCombo->setToolTip(
        tr("По переходной: time, value.\n"
           "По клапану/сигналу: time, valve, value (табы/пробелы/; и т.п.)."));

    connect(ui->openFileButton, &QPushButton::clicked, this, &IdTab::openFile);
    connect(ui->identifyButton, &QPushButton::clicked, this, &IdTab::runIdentification);
    connect(ui->clearButton, &QPushButton::clicked, this, &IdTab::clearAll);
    connect(ui->settingsButton, &QPushButton::clicked, this, &IdTab::openSettings);
    connect(ui->idSettingsButton, &QPushButton::clicked, this, &IdTab::openIdSettings);
    connect(ui->methodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        if (has_data_)
            ui->fileLabel->setText(tr("%1 (нажмите ▶ для пересчёта)").arg(QFileInfo(file_path_).fileName()));
    });
}

IdTab::~IdTab() {
    delete ui;
}

void IdTab::install_custom_widgets() {
    display_          = new TfDisplayWidget(QStringLiteral("W(p) = "), ui->formHost);
    auto* form_layout = new QVBoxLayout(ui->formHost);
    form_layout->setContentsMargins(0, 0, 0, 0);
    form_layout->addWidget(display_, 0, Qt::AlignLeft | Qt::AlignVCenter);

    metrics_             = new RegulationWidget(3, 2, ui->metricsHost);
    auto* metrics_layout = new QVBoxLayout(ui->metricsHost);
    metrics_layout->setContentsMargins(0, 0, 0, 0);
    metrics_layout->addWidget(metrics_, 0, Qt::AlignRight | Qt::AlignVCenter);

    ui->topLayout->setStretch(0, 0);
    ui->topLayout->setStretch(1, 1);
    ui->topLayout->setStretch(2, 0);
}

void IdTab::setup_metrics() {
    metrics_->setLabels(
        {
            QStringLiteral("t<sub>р</sub>:"),
            QStringLiteral("ω<sub>n</sub>:"),
            QStringLiteral("t<sub>н</sub>:"),
            QStringLiteral("ω<sub>c</sub>:"),
            QStringLiteral("ζ:"),
            QStringLiteral("h<sub>уст</sub>:"),
        },
        {
            tr("Время регулирования, с"),
            tr("Собственная частота, рад/с"),
            tr("Время нарастания, с"),
            tr("Частота среза, рад/с"),
            tr("Коэффициент демпфирования, %"),
            tr("Установившееся значение"),
        });
    metrics_->setColors({{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}});
}

void IdTab::show_error(const QString& message) {
    QMessageBox::critical(this, tr("Ошибка"), message);
}

void IdTab::openSettings() {
    ModParDialog dialog(model_param_, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    model_param_ = dialog.data();
    // Grid/Padé-sample settings only: recompute responses, keep identified plant.
    if (!charts_->empty()) {
        charts_->recomputeAll(model_param_);
        if (charts_->hasLastQuality()) {
            const auto& q = charts_->lastQuality();
            if (q.is_settled) {
                metrics_->updateValues({q.settling_time, q.natural_frequency, q.rise_time, q.cut_frequency,
                                        q.damping_ratio, q.steady_state});
            }
            else {
                metrics_->updateValues({});
            }
        }
    }
}

void IdTab::openIdSettings() {
    IdSettingsDialog dialog(id_settings_, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    id_settings_ = dialog.data();
    if (has_data_)
        runIdentification();
}

void IdTab::openFile() {
    const QString path = QFileDialog::getOpenFileName(this, tr("Открыть файл данных"), {},
                                                      tr("Файлы данных (*.txt *.csv *.dat);;Все файлы (*)"));
    if (path.isEmpty())
        return;
    file_path_ = path;
    has_data_  = false; // set true only after successful load in runIdentification
    ui->fileLabel->setText(QFileInfo(path).fileName());
    ui->fileLabel->setToolTip(path);
    runIdentification();
}

void IdTab::clearAll() {
    has_data_ = false;
    file_path_.clear();
    step_series_.clear();
    valve_series_.clear();
    signal_series_.clear();
    display_->clear();
    charts_->clearAll();
    metrics_->updateValues({});
    ui->fileLabel->setText(tr("Файл не выбран"));
    ui->fileLabel->setToolTip({});
}

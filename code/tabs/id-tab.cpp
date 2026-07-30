#include "code/tabs/id-tab.h"
#include "ui_id-tab.h"

#include "code/dialogs/id-settings-dialog.h"
#include "code/dialogs/mod-par-dialog.h"
#include "code/util/data-file-parser.hpp"
#include "code/util/tf-builder.hpp"

#include "numina/classes/control/duhamel-solver.h"
#include "numina/classes/control/simoyu-identifier.h"
#include "numina/classes/control/transfer-function/response-lab.h"

#include <QComboBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QVBoxLayout>

#include <algorithm>
#include <utility>

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
    vis.impulse = vis.nyquist = vis.amplitude = vis.phase = false;
    charts_->setVisibility(vis);

    chartsMenu_ = new QMenu(this);
    connect(chartsMenu_, &QMenu::aboutToShow, this, [this] { charts_->populateMenu(chartsMenu_); });

    auto* charts_btn = new QToolButton(this);
    charts_btn->setObjectName(QStringLiteral("chartsButton"));
    charts_btn->setText(QStringLiteral("◫"));
    charts_btn->setToolTip(tr("Отображаемые графики"));
    charts_btn->setPopupMode(QToolButton::InstantPopup);
    charts_btn->setMenu(chartsMenu_);
    charts_btn->setFixedSize(40, 40);
    ui->buttonLayout->insertWidget(ui->buttonLayout->indexOf(ui->idSettingsButton), charts_btn);

    ui->methodCombo->setToolTip(tr(
        "По переходной: time, value.\n"
        "По клапану/сигналу: time, valve, value (табы/пробелы/; и т.п.)."));

    connect(ui->openFileButton, &QPushButton::clicked, this, &IdTab::openFile);
    connect(ui->identifyButton, &QPushButton::clicked, this, &IdTab::runIdentification);
    connect(ui->clearButton, &QPushButton::clicked, this, &IdTab::clearAll);
    connect(ui->settingsButton, &QPushButton::clicked, this, &IdTab::openSettings);
    connect(ui->idSettingsButton, &QPushButton::clicked, this, &IdTab::openIdSettings);
    connect(ui->methodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        if (hasData_)
            ui->fileLabel->setText(tr("%1 (нажмите ▶ для пересчёта)").arg(QFileInfo(filePath_).fileName()));
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
    metrics_->setLabels({
        QStringLiteral("t<sub>р</sub>:"),
        QStringLiteral("ω<sub>n</sub>:"),
        QStringLiteral("t<sub>н</sub>:"),
        QStringLiteral("ω<sub>c</sub>:"),
        QStringLiteral("ζ:"),
        QStringLiteral("h<sub>уст</sub>:"),
    });
    metrics_->setPrecisions({0, 0, 0, 0, 0, 0});
    metrics_->setColors({{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}});
}

void IdTab::show_error(const QString& message) {
    QMessageBox::critical(this, tr("Ошибка"), message);
}

void IdTab::openSettings() {
    ModParDialog dialog(modelParam_, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    modelParam_ = dialog.data();
    if (hasData_ && !display_->isEmpty())
        runIdentification();
}

void IdTab::openIdSettings() {
    IdSettingsDialog dialog(idSettings_, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    idSettings_ = dialog.data();
    if (hasData_)
        runIdentification();
}

void IdTab::openFile() {
    const QString path = QFileDialog::getOpenFileName(this, tr("Открыть файл данных"), {},
                                                      tr("Файлы данных (*.txt *.csv *.dat);;Все файлы (*)"));
    if (path.isEmpty())
        return;
    filePath_ = path;
    hasData_  = true;
    ui->fileLabel->setText(QFileInfo(path).fileName());
    ui->fileLabel->setToolTip(path);
    runIdentification();
}

bool IdTab::load_step_file(const QString& path) {
    auto opt = data_file_parser::readStepResponse(path);
    if (!opt) {
        show_error(tr("Не удалось прочитать пары time, value.\n"
                      "Числа с «.»/«,» и научной записью; мусор отбрасывается."));
        return false;
    }
    stepSeries_ = std::move(*opt);
    valveSeries_.clear();
    signalSeries_.clear();
    return true;
}

bool IdTab::load_valve_signal_file(const QString& path) {
    const auto numbers = data_file_parser::extractNumbersFromFile(path);
    if (numbers.size() < 6) {
        show_error(tr("Файл слишком короткий для time, valve, value "
                      "(найдено чисел: %1).")
                       .arg(numbers.size()));
        return false;
    }
    data_file_parser::VecPair valve, signal;
    if (!data_file_parser::asValveSignal(numbers, valve, signal)) {
        show_error(tr("Не удалось разобрать тройки time, valve, value.\n"
                      "Чисел: %1 (нужно кратно 3, минимум 6).")
                       .arg(numbers.size()));
        return false;
    }
    valveSeries_  = std::move(valve);
    signalSeries_ = std::move(signal);
    stepSeries_.clear();
    return true;
}

void IdTab::runIdentification() {
    if (filePath_.isEmpty()) {
        show_error(tr("Сначала выберите файл с экспериментальными данными."));
        return;
    }

    const auto method = static_cast<Method>(ui->methodCombo->currentIndex());
    const bool loaded =
        (method == Method::StepResponse) ? load_step_file(filePath_) : load_valve_signal_file(filePath_);
    if (!loaded)
        return;
    hasData_ = true;

    try {
        numina::SimoyuIdentifier simoyu;
        Series experimental_h;
        const bool auto_order = idSettings_.autoOrder;
        const std::size_t den_n =
            static_cast<std::size_t>(std::clamp(idSettings_.denOrder, 1, 12));
        const std::size_t num_m = static_cast<std::size_t>(
            std::clamp(idSettings_.numOrder, 0, static_cast<int>(den_n)));
        const std::size_t max_order =
            static_cast<std::size_t>(std::clamp(modelParam_.approxOrder, 2, 12));
        const bool want_tau = idSettings_.estimateTau;

        if (method == Method::StepResponse) {
            if (stepSeries_.size() < 2) {
                show_error(tr("Недостаточно точек переходной характеристики."));
                return;
            }
            experimental_h = stepSeries_;
        } else {
            if (valveSeries_.size() < 2 || signalSeries_.size() < 2) {
                show_error(tr("Недостаточно точек клапана/сигнала (строк: %1).")
                               .arg(valveSeries_.size()));
                return;
            }
            for (std::size_t i = 1; i < valveSeries_.size(); ++i) {
                if (valveSeries_[i].first < valveSeries_[i - 1].first) {
                    show_error(tr("Время должно быть неубывающим (нарушение около точки %1).").arg(i));
                    return;
                }
            }
            numina::DuhamelSolver duhamel;
            experimental_h = duhamel.stepResponse(valveSeries_, signalSeries_);
            if (experimental_h.size() < 2) {
                show_error(tr("Дюамель не восстановил h(t) (точек: %1).\n"
                              "Проверьте, что u(t) меняется и y(t) согласован по времени.")
                               .arg(experimental_h.size()));
                return;
            }
            stepSeries_ = experimental_h;
        }

        numina::SimoyuIdentifier::DelayFit fit;
        if (want_tau) {
            if (auto_order)
                fit = simoyu.identifyDelayAuto(experimental_h, max_order, max_order);
            else
                fit = simoyu.identifyDelay(experimental_h, den_n, num_m);
        } else {
            fit.tau = 0.0;
            if (auto_order)
                fit.plant = simoyu.identifyAuto(experimental_h, max_order, max_order);
            else
                fit.plant = simoyu.identify(experimental_h, den_n, num_m);
        }

        if (!fit.ok()) {
            show_error(tr("Идентификация не дала модели (deg D < 1).\n"
                          "Точек h(t): %1. Проверьте данные и метод.")
                           .arg(experimental_h.size()));
            return;
        }

        identifiedTf_  = fit.plant;
        identifiedTau_ = fit.tau > 0.0 ? fit.tau : 0.0;
        apply_result(fit.plant, identifiedTau_, experimental_h);
        ui->fileLabel->setText(QFileInfo(filePath_).fileName());
    } catch (const std::exception& ex) {
        show_error(tr("Ошибка идентификации: %1").arg(QString::fromUtf8(ex.what())));
    }
}

void IdTab::apply_result(const numina::TransferFunction& plant, double tau, const Series& experimental_h) {
    numina::TransferFunction model = plant;
    if (tau > 0.0)
        model *= numina::TransferFunction::makeDelay(tau, static_cast<std::uint8_t>(modelParam_.approxOrder));

    display_->setTransferFunction(plant, tau);

    charts_->clearAll();
    charts_->appendTransientCurve(experimental_h, tr("Эксперимент"));
    charts_->appendFromTf(model, modelParam_, tr("Модель"));

    try {
        numina::ResponseLab lab(model);
        const auto q = lab.evaluate();
        if (q.is_settled) {
            metrics_->updateValues({q.settling_time, q.natural_frequency, q.rise_time, q.cut_frequency,
                                    q.damping_ratio, q.steady_state});
        } else {
            metrics_->updateValues({});
        }
    } catch (...) {
        metrics_->updateValues({});
    }
}

void IdTab::clearAll() {
    hasData_ = false;
    filePath_.clear();
    stepSeries_.clear();
    valveSeries_.clear();
    signalSeries_.clear();
    identifiedTf_  = {};
    identifiedTau_ = 0.0;
    display_->clear();
    charts_->clearAll();
    metrics_->updateValues({});
    ui->fileLabel->setText(tr("Файл не выбран"));
    ui->fileLabel->setToolTip({});
}

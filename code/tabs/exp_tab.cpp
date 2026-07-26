#include "code/tabs/exp_tab.h"
#include "ui_exp_tab.h"

#include "code/dialogs/mod_par_dialog.h"
#include "code/util/data_file_parser.hpp"
#include "code/util/tf_builder.hpp"

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

ExpTab::ExpTab(QWidget* parent) : QWidget(parent), ui(new Ui::ExpTab) {
    ui->setupUi(this);
    installCustomWidgets();
    setupMetrics();

    charts_ = new ResponseChartBank(this);
    ui->chartsLayout->addWidget(charts_);
    ui->verticalLayout->setStretch(0, 0);
    ui->verticalLayout->setStretch(1, 0);
    ui->verticalLayout->setStretch(2, 1);

    chartsMenu_ = new QMenu(this);
    connect(chartsMenu_, &QMenu::aboutToShow, this, [this] { charts_->populateMenu(chartsMenu_); });

    auto* chartsBtn = new QToolButton(this);
    chartsBtn->setObjectName(QStringLiteral("chartsButton"));
    chartsBtn->setText(QStringLiteral("◫"));
    chartsBtn->setToolTip(tr("Отображаемые графики"));
    chartsBtn->setPopupMode(QToolButton::InstantPopup);
    chartsBtn->setMenu(chartsMenu_);
    chartsBtn->setFixedSize(36, 36);
    ui->buttonLayout->insertWidget(ui->buttonLayout->indexOf(ui->settingsButton), chartsBtn);

    ui->methodCombo->setToolTip(tr(
        "По переходной: файл time, value.\n"
        "По клапану/сигналу: файл time, valve, value → h(t) через Дюамель, затем Симою."));

    connect(ui->openFileButton, &QPushButton::clicked, this, &ExpTab::openFile);
    connect(ui->identifyButton, &QPushButton::clicked, this, &ExpTab::runIdentification);
    connect(ui->clearButton, &QPushButton::clicked, this, &ExpTab::clearAll);
    connect(ui->settingsButton, &QPushButton::clicked, this, &ExpTab::openSettings);
    connect(ui->methodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        // Method change invalidates typed interpretation of the same file — require re-open or re-run.
        if (hasData_)
            ui->fileLabel->setText(tr("%1 (нажмите ▶ для пересчёта)").arg(QFileInfo(filePath_).fileName()));
    });
}

ExpTab::~ExpTab() {
    delete ui;
}

void ExpTab::installCustomWidgets() {
    display_ = new TfDisplayWidget(QStringLiteral("W(p) = "), ui->formHost);
    auto* formLayout = new QVBoxLayout(ui->formHost);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->addWidget(display_);

    metrics_ = new RegulationWidget(3, 2, ui->metricsHost);
    auto* metricsLayout = new QVBoxLayout(ui->metricsHost);
    metricsLayout->setContentsMargins(0, 0, 0, 0);
    metricsLayout->addWidget(metrics_);
}

void ExpTab::setupMetrics() {
    metrics_->setLabels({
        QStringLiteral("t<sub>р</sub>:"),
        QStringLiteral("ω<sub>n</sub>:"),
        QStringLiteral("t<sub>н</sub>:"),
        QStringLiteral("ω<sub>c</sub>:"),
        QStringLiteral("ζ:"),
        QStringLiteral("h<sub>уст</sub>:"),
    });
    metrics_->setPrecisions({2, 4, 2, 4, 4, 2});
    metrics_->setColors({{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}});
}

void ExpTab::showError(const QString& message) {
    QMessageBox::critical(this, tr("Ошибка"), message);
}

void ExpTab::showInfo(const QString& message) {
    QMessageBox::information(this, tr("Идентификация"), message);
}

void ExpTab::openSettings() {
    ModParDialog dialog(modelParam_, this);
    if (dialog.exec() == QDialog::Accepted)
        modelParam_ = dialog.data();
}

void ExpTab::openFile() {
    const QString fileName =
        QFileDialog::getOpenFileName(this, tr("Открыть файл данных"), {},
                                     tr("Файлы данных (*.txt *.csv *.dat);;Все файлы (*)"));
    if (fileName.isEmpty())
        return;

    const auto method = static_cast<Method>(ui->methodCombo->currentIndex());
    bool ok = false;
    if (method == Method::StepResponse)
        ok = loadStepFile(fileName);
    else
        ok = loadValveSignalFile(fileName);

    if (!ok)
        return;

    filePath_ = fileName;
    hasData_  = true;
    ui->fileLabel->setText(QFileInfo(fileName).fileName());
    ui->fileLabel->setToolTip(fileName);

    // Load + identify in one step
    runIdentification();
}

bool ExpTab::loadStepFile(const QString& path) {
    auto opt = data_file_parser::readStepResponse(path);
    if (!opt) {
        showError(tr("Не удалось прочитать пары time, value из файла.\n"
                     "Ожидаются числа (точка или запятая как десятичный разделитель);\n"
                     "мусор и лишние разделители отбрасываются."));
        return false;
    }
    stepSeries_   = std::move(*opt);
    valveSeries_.clear();
    signalSeries_.clear();
    return true;
}

bool ExpTab::loadValveSignalFile(const QString& path) {
    auto opt = data_file_parser::readValveSignal(path);
    if (!opt) {
        showError(tr("Не удалось прочитать тройки time, valve, value из файла.\n"
                     "Ожидаются числа (точка или запятая как десятичный разделитель);\n"
                     "мусор и лишние разделители отбрасываются."));
        return false;
    }
    valveSeries_  = std::move(opt->first);
    signalSeries_ = std::move(opt->second);
    stepSeries_.clear();
    return true;
}

void ExpTab::runIdentification() {
    if (!hasData_ && filePath_.isEmpty()) {
        showError(tr("Сначала выберите файл с экспериментальными данными."));
        return;
    }

    const auto method = static_cast<Method>(ui->methodCombo->currentIndex());

    // Always re-read from disk so a method switch uses the correct column layout.
    if (!filePath_.isEmpty()) {
        const bool ok =
            (method == Method::StepResponse) ? loadStepFile(filePath_) : loadValveSignalFile(filePath_);
        if (!ok)
            return;
        hasData_ = true;
    }

    try {
        numina::SimoyuIdentifier simoyu;
        std::vector<std::pair<double, double>> experimentalH;
        const std::size_t maxOrder =
            static_cast<std::size_t>(std::max(2, std::min(modelParam_.approxOrder, 12)));

        if (method == Method::StepResponse) {
            if (stepSeries_.size() < 2) {
                showError(tr("Недостаточно точек переходной характеристики."));
                return;
            }
            experimentalH = stepSeries_;
            identifiedTf_ = simoyu.identifyAuto(experimentalH, maxOrder, maxOrder);
        } else {
            if (valveSeries_.size() < 2 || signalSeries_.size() < 2) {
                showError(tr("Недостаточно точек клапана/сигнала."));
                return;
            }
            numina::DuhamelSolver duhamel;
            experimentalH = duhamel.stepResponse(valveSeries_, signalSeries_);
            if (experimentalH.size() < 2) {
                showError(tr("Не удалось восстановить h(t) по клапану и сигналу (Дюамель)."));
                return;
            }
            stepSeries_   = experimentalH;
            identifiedTf_ = simoyu.identifyAuto(experimentalH, maxOrder, maxOrder);
        }

        if (!identifiedTf_.hasPoles() && identifiedTf_.getDenominator().degree() < 1) {
            showError(tr("Идентификация не дала устойчивой модели. Проверьте данные и метод."));
            return;
        }

        applyIdentificationResult(identifiedTf_, experimentalH);
        ui->fileLabel->setText(QFileInfo(filePath_).fileName());
    } catch (const std::exception& ex) {
        showError(tr("Ошибка идентификации: %1").arg(QString::fromUtf8(ex.what())));
    }
}

void ExpTab::applyIdentificationResult(const numina::TransferFunction& tf,
                                       const std::vector<std::pair<double, double>>& experimentalH) {
    display_->setTransferFunction(tf);

    charts_->clearAll();
    // Experimental first, then model — overlay comparison on transient chart
    charts_->appendTransientCurve(experimentalH, tr("Эксперимент"));
    charts_->appendFromTf(tf, modelParam_, tr("Модель"));

    try {
        numina::ResponseLab lab(tf);
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

void ExpTab::clearAll() {
    hasData_ = false;
    filePath_.clear();
    stepSeries_.clear();
    valveSeries_.clear();
    signalSeries_.clear();
    identifiedTf_ = {};
    display_->clear();
    charts_->clearAll();
    metrics_->updateValues({});
    ui->fileLabel->setText(tr("Файл не выбран"));
    ui->fileLabel->setToolTip({});
}

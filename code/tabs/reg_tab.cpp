#include "code/tabs/reg_tab.h"
#include "ui_reg_tab.h"

#include "code/charts/chart_utils.hpp"
#include "code/control/regulator_factory.hpp"
#include "code/dialogs/help_dialog.h"
#include "code/dialogs/mod_par_dialog.h"
#include "code/util/tf_builder.hpp"

#include "numina/classes/control/transfer-function/response-lab.h"

#include <QCheckBox>
#include <QMessageBox>
#include <QVBoxLayout>

RegTab::RegTab(QWidget* parent) : QWidget(parent), ui(new Ui::RegTab) {
    ui->setupUi(this);
    installCustomWidgets();
    setupMetrics();

    chartTran_ = new QChart;
    chartFreq_ = new QChart;
    ui->chartsLayout->addWidget(chart_utils::makeChartView(
        chartTran_, this, tr("Переходный процесс"), tr("Время t, секунды"), tr("h(t)")));
    ui->chartsLayout->addWidget(chart_utils::makeChartView(
        chartFreq_, this, tr("Комплексно-частотная характеристика (КЧХ)"), tr("Реальная ось"),
        tr("Мнимая ось")));

    form_->setTransferFunction(&currentTf_);

    // Cap recompute to display refresh (60 / 144 / … Hz from QScreen).
    debounce_.setSingleShot(true);
    debounce_.setTimerType(Qt::PreciseTimer);
    debounce_.setInterval(chart_utils::frameIntervalMs());
    connect(&debounce_, &QTimer::timeout, this, &RegTab::replaceTransferFunction);

    connect(ui->helpButton, &QPushButton::clicked, this, &RegTab::openHelp);
    connect(ui->settingsButton, &QPushButton::clicked, this, &RegTab::openSettings);
    connect(ui->addButton, &QPushButton::clicked, this, &RegTab::addTransferFunction);
    connect(ui->clearButton, &QPushButton::clicked, this, &RegTab::clearCharts);
}

RegTab::~RegTab() {
    delete ui;
}

void RegTab::installCustomWidgets() {
    form_ = new TranFuncForm(6, 6, QStringLiteral("W<sub>ОУ</sub>(p) = "), ui->formHost);
    auto* formLayout = new QVBoxLayout(ui->formHost);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->addWidget(form_);

    metrics_ = new RegulationWidget(3, 4, ui->metricsHost);
    auto* metricsLayout = new QVBoxLayout(ui->metricsHost);
    metricsLayout->setContentsMargins(0, 0, 0, 0);
    metricsLayout->addWidget(metrics_);

    parameters_ = {
        new RegParameter(QStringLiteral("K<sub>p</sub>"), 0.05, 2000, 0.05, 5, this),
        new RegParameter(QStringLiteral("T<sub>u</sub>"), 0.05, 2000, 1, 120, this),
        new RegParameter(QStringLiteral("T<sub>d</sub>"), 0.05, 2000, 1, 60, this),
    };
    for (auto* p : parameters_) {
        ui->paramsLayout->addLayout(p->layout());
        connect(p->checkBox(), &QCheckBox::toggled, this, &RegTab::scheduleReplace);
        connect(p->slider(), &DoubleSlider::doubleValueChanged, this, &RegTab::scheduleReplace);
    }
}

void RegTab::setupMetrics() {
    metrics_->setLabels({
        QStringLiteral("t<sub>р</sub>:"), QStringLiteral("ω<sub>n</sub>:"), QStringLiteral("h<sub>уст</sub>:"),
        QStringLiteral("ЛИК:"),           QStringLiteral("t<sub>н</sub>:"),  QStringLiteral("ω<sub>c</sub>:"),
        QStringLiteral("σ<sub>ст</sub>:"), QStringLiteral("ИКК:"),          QStringLiteral("t<sub>п</sub>:"),
        QStringLiteral("ζ:"),             QStringLiteral("σ<sub>пр</sub>:"), QStringLiteral("СКО:"),
    });
    metrics_->setPrecisions({2, 4, 2, 4, 2, 4, 2, 4, 2, 4, 2, 4});
    metrics_->setColors({{1, 2}, {0, 0}, {0, 0}, {1, 2}, {1, 2}, {0, 0}, {0, 0}, {1, 2}, {1, 2}, {2, 1}, {1, 2}, {1, 2}});
}

void RegTab::showError(const QString& message) {
    QMessageBox::critical(this, tr("Ошибка ввода"), message);
}

void RegTab::openSettings() {
    ModParDialog dialog(modelParam_, this);
    if (dialog.exec() == QDialog::Accepted)
        modelParam_ = dialog.data();
}

void RegTab::openHelp() {
    HelpDialog dialog(this);
    dialog.exec();
}

void RegTab::scheduleReplace() {
    if (hasSeries_) {
        // Restart coalescing timer: at most one recompute per display frame.
        debounce_.setInterval(chart_utils::frameIntervalMs());
        debounce_.start();
    }
}

void RegTab::applyCurrentRegulator(bool replaceLast) {
    auto plantNum = form_->numerator();
    auto plantDen = form_->denominator();
    if (plantDen.empty())
        return;

    const auto reg = regulator_factory::make(parameters_[0]->enabled(), parameters_[1]->enabled(),
                                             parameters_[2]->enabled(), parameters_[0]->value(),
                                             parameters_[1]->value(), parameters_[2]->value());

    try {
        currentTf_ =
            tf_builder::closedLoop(std::move(plantNum), std::move(plantDen), reg.num, reg.den,
                                   form_->hasDelay() ? form_->delayTime() : 0.0, modelParam_.approxOrder);
        form_->setTransferFunction(&currentTf_);

        auto h = tf_builder::transient(currentTf_, modelParam_);
        auto w = tf_builder::frequency(currentTf_, modelParam_);
        const QString title = QString::fromStdString(reg.title);

        if (replaceLast && hasSeries_) {
            if (!tranSeries_.empty())
                tranSeries_.pop_back();
            if (!freqSeries_.empty())
                freqSeries_.pop_back();
            tranSeries_.push_back(Series(h));
            freqSeries_.push_back(ComplexSeries(w));
            const std::size_t penIndex = seriesIndex_ > 0 ? seriesIndex_ - 1 : 0;
            if (!chart_utils::replaceLastRealSeries(chartTran_, h, title, true))
                chart_utils::addRealSeries(chartTran_, h, title, penIndex, true);
            if (!chart_utils::replaceLastComplexSeries(chartFreq_, w, title, true))
                chart_utils::addComplexSeries(chartFreq_, w, title, penIndex, true);
        } else {
            tranSeries_.push_back(Series(h));
            freqSeries_.push_back(ComplexSeries(w));
            chart_utils::addRealSeries(chartTran_, h, title, seriesIndex_, true);
            chart_utils::addComplexSeries(chartFreq_, w, title, seriesIndex_, true);
            ++seriesIndex_;
            hasSeries_ = true;
        }

        numina::ResponseLab lab(currentTf_);
        const auto q = lab.evaluate();
        if (q.is_settled) {
            metrics_->updateValues({
                q.settling_time,
                q.natural_frequency,
                q.steady_state,
                q.iae,
                q.rise_time,
                q.cut_frequency,
                1.0 - q.steady_state,
                q.ise,
                q.peak_time,
                q.damping_ratio,
                q.overshoot_percent,
                q.sigma,
            });
        } else {
            metrics_->updateValues({});
        }

        chart_utils::updateAxes(chartTran_, {tranSeries_.min_x(), tranSeries_.max_x()},
                                chart_utils::computeAxesRange(tranSeries_.min_y(), tranSeries_.max_y()));
        chart_utils::updateAxes(chartFreq_, chart_utils::computeAxesRange(freqSeries_.min_x(), freqSeries_.max_x()),
                                chart_utils::computeAxesRange(freqSeries_.min_y(), freqSeries_.max_y()));
    } catch (const std::exception& ex) {
        showError(QString::fromUtf8(ex.what()));
    }
}

void RegTab::addTransferFunction() {
    applyCurrentRegulator(false);
}

void RegTab::replaceTransferFunction() {
    if (!hasSeries_)
        return;
    applyCurrentRegulator(true);
}

void RegTab::clearCharts() {
    chart_utils::removeAllSeries(chartTran_);
    chart_utils::removeAllSeries(chartFreq_);
    tranSeries_.clear();
    freqSeries_.clear();
    seriesIndex_ = 0;
    hasSeries_ = false;
    metrics_->updateValues({});
}

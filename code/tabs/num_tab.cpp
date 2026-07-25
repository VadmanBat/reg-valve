#include "code/tabs/num_tab.h"
#include "ui_num_tab.h"

#include "code/charts/chart_utils.hpp"
#include "code/dialogs/mod_par_dialog.h"
#include "code/util/tf_builder.hpp"

#include "numina/classes/control/transfer-function/response-lab.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>

NumTab::NumTab(QWidget* parent) : QWidget(parent), ui(new Ui::NumTab) {
    ui->setupUi(this);
    installCustomWidgets();
    setupMetrics();

    chartTran_ = new QChart;
    chartFreq_ = new QChart;
    ui->chartsLayout->addWidget(chart_utils::makeChartView(
        chartTran_, this, tr("Переходная характеристика"), tr("Время t, секунды"),
        tr("Параметр h(t)")));
    ui->chartsLayout->addWidget(chart_utils::makeChartView(
        chartFreq_, this, tr("Комплексно-частотная характеристика (КЧХ)"), tr("Реальная ось"),
        tr("Мнимая ось")));

    form_->bindNameLabel(ui->nameLabel);
    form_->setTransferFunction(&currentTf_);

    connect(ui->settingsButton, &QPushButton::clicked, this, &NumTab::openSettings);
    connect(ui->addButton, &QPushButton::clicked, this, &NumTab::addTransferFunction);
    connect(ui->replaceButton, &QPushButton::clicked, this, &NumTab::replaceTransferFunction);
    connect(ui->clearButton, &QPushButton::clicked, this, &NumTab::clearCharts);
}

NumTab::~NumTab() {
    delete ui;
}

void NumTab::installCustomWidgets() {
    form_ = new TranFuncForm(6, 6, QStringLiteral("W(p) = "), ui->formHost);
    auto* formLayout = new QVBoxLayout(ui->formHost);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->addWidget(form_);

    metrics_ = new RegulationWidget(3, 2, ui->metricsHost);
    auto* metricsLayout = new QVBoxLayout(ui->metricsHost);
    metricsLayout->setContentsMargins(0, 0, 0, 0);
    metricsLayout->addWidget(metrics_);
}

void NumTab::setupMetrics() {
    metrics_->setLabels({
        QStringLiteral("t<sub>р</sub>:"), QStringLiteral("ω<sub>n</sub>:"),
        QStringLiteral("t<sub>н</sub>:"), QStringLiteral("ω<sub>c</sub>:"),
        QStringLiteral("ζ:"),             QStringLiteral("h<sub>уст</sub>:"),
    });
    metrics_->setPrecisions({2, 4, 2, 4, 4, 2});
    metrics_->setColors({{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}});
}

void NumTab::showError(const QString& message) {
    QMessageBox::critical(this, tr("Ошибка ввода"), message);
}

void NumTab::openSettings() {
    ModParDialog dialog(modelParam_, this);
    if (dialog.exec() == QDialog::Accepted)
        modelParam_ = dialog.data();
}

void NumTab::addTransferFunction() {
    auto num = form_->numerator();
    auto den = form_->denominator();
    if (!tf_builder::validInput(num, den)) {
        if (den.empty())
            showError(tr("Знаменатель НЕ может быть равен нулю!"));
        else if (den.size() == 1)
            showError(tr("Порядок знаменателя НЕ может быть меньше первого!"));
        else
            showError(tr("Порядок числителя НЕ может быть больше порядка знаменателя!"));
        return;
    }

    try {
        currentTf_ = tf_builder::plant(std::move(num), std::move(den), form_->hasDelay() ? form_->delayTime() : 0.0,
                                       modelParam_.approxOrder);
        form_->setTransferFunction(&currentTf_);

        auto h = tf_builder::transient(currentTf_, modelParam_);
        auto w = tf_builder::frequency(currentTf_, modelParam_);

        tranSeries_.push_back(Series(h));
        freqSeries_.push_back(ComplexSeries(w));

        chart_utils::addRealSeries(chartTran_, h, form_->linkName(), seriesIndex_, true);
        chart_utils::addComplexSeries(chartFreq_, w, form_->linkName(), seriesIndex_, true);
        ++seriesIndex_;

        numina::ResponseLab lab(currentTf_);
        const auto q = lab.evaluate();
        if (q.is_settled) {
            metrics_->updateValues({q.settling_time, q.natural_frequency, q.rise_time, q.cut_frequency,
                                    q.damping_ratio, q.steady_state});
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

void NumTab::replaceTransferFunction() {
    if (!tranSeries_.empty()) {
        chart_utils::eraseLastSeries(chartTran_);
        tranSeries_.pop_back();
        if (seriesIndex_ > 0)
            --seriesIndex_;
    }
    if (!freqSeries_.empty()) {
        chart_utils::eraseLastSeries(chartFreq_);
        freqSeries_.pop_back();
    }
    addTransferFunction();
}

void NumTab::clearCharts() {
    chart_utils::removeAllSeries(chartTran_);
    chart_utils::removeAllSeries(chartFreq_);
    tranSeries_.clear();
    freqSeries_.clear();
    seriesIndex_ = 0;
    metrics_->updateValues({});
}

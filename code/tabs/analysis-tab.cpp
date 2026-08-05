#include "code/tabs/analysis-tab.h"

#include "code/dialogs/mod-par-dialog.h"
#include "code/tabs/tab-charts-button.hpp"
#include "code/util/tf-builder.hpp"
#include "ui_analysis-tab.h"

#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

AnalysisTab::AnalysisTab(QWidget* parent) : QWidget(parent), ui(new Ui::AnalysisTab) {
    ui->setupUi(this);
    install_custom_widgets();
    setup_metrics();

    charts_ = new ResponseChartBank(this);
    ui->chartsLayout->addWidget(charts_);
    ui->verticalLayout->setStretch(0, 0);
    ui->verticalLayout->setStretch(1, 0);
    ui->verticalLayout->setStretch(2, 1);

    charts_menu_     = new QMenu(this);
    auto* charts_btn = tab_ui::makeChartsButton(this, charts_, charts_menu_);
    ui->buttonLayout->insertWidget(ui->buttonLayout->indexOf(ui->settingsButton), charts_btn);

    form_->bindNameLabel(ui->nameLabel);
    form_->setTransferFunction(&current_tf_);

    connect(ui->settingsButton, &QPushButton::clicked, this, &AnalysisTab::openSettings);
    connect(ui->addButton, &QPushButton::clicked, this, &AnalysisTab::addTransferFunction);
    connect(ui->replaceButton, &QPushButton::clicked, this, &AnalysisTab::replaceTransferFunction);
    connect(ui->clearButton, &QPushButton::clicked, this, &AnalysisTab::clearCharts);
}

AnalysisTab::~AnalysisTab() {
    delete ui;
}

void AnalysisTab::install_custom_widgets() {
    form_             = new TranFuncForm(6, 6, QStringLiteral("W(p) = "), ui->formHost);
    auto* form_layout = new QVBoxLayout(ui->formHost);
    form_layout->setContentsMargins(0, 0, 0, 0);
    form_layout->addWidget(form_, 0, Qt::AlignLeft | Qt::AlignVCenter);

    metrics_             = new RegulationWidget(3, 2, ui->metricsHost);
    auto* metrics_layout = new QVBoxLayout(ui->metricsHost);
    metrics_layout->setContentsMargins(0, 0, 0, 0);
    metrics_layout->addWidget(metrics_, 0, Qt::AlignRight | Qt::AlignVCenter);

    ui->topLayout->insertStretch(1, 1);
}

void AnalysisTab::setup_metrics() {
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

void AnalysisTab::show_error(const QString& message) {
    QMessageBox::critical(this, tr("Ошибка ввода"), message);
}

void AnalysisTab::update_metrics() {
    if (!charts_->hasLastQuality() || !charts_->lastQuality().is_settled) {
        metrics_->updateValues({});
        return;
    }
    const auto& q = charts_->lastQuality();
    metrics_->updateValues(
        {q.settling_time, q.natural_frequency, q.rise_time, q.cut_frequency, q.damping_ratio, q.steady_state});
}

void AnalysisTab::openSettings() {
    ModParDialog dialog(model_param_, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    model_param_ = dialog.data();
    if (charts_->empty())
        return;
    try {
        charts_->recomputeAll(model_param_);
        update_metrics();
    }
    catch (const std::exception& ex) {
        show_error(QString::fromUtf8(ex.what()));
    }
}

void AnalysisTab::addTransferFunction() {
    auto num = form_->numerator();
    auto den = form_->denominator();
    if (!tf_builder::validInput(num, den)) {
        if (den.empty())
            show_error(tr("Знаменатель НЕ может быть равен нулю!"));
        else if (den.size() == 1)
            show_error(tr("Порядок знаменателя НЕ может быть меньше первого!"));
        else
            show_error(tr("Порядок числителя НЕ может быть больше порядка знаменателя!"));
        return;
    }

    try {
        current_tf_ = tf_builder::plant(std::move(num), std::move(den), form_->delayTime(), model_param_.approxOrder);
        form_->setTransferFunction(&current_tf_);
        charts_->appendFromTf(current_tf_, model_param_, form_->linkName());
        update_metrics();
    }
    catch (const std::exception& ex) {
        show_error(QString::fromUtf8(ex.what()));
    }
}

void AnalysisTab::replaceTransferFunction() {
    if (charts_->empty()) {
        addTransferFunction();
        return;
    }
    auto num = form_->numerator();
    auto den = form_->denominator();
    if (!tf_builder::validInput(num, den)) {
        if (den.empty())
            show_error(tr("Знаменатель НЕ может быть равен нулю!"));
        else if (den.size() == 1)
            show_error(tr("Порядок знаменателя НЕ может быть меньше первого!"));
        else
            show_error(tr("Порядок числителя НЕ может быть больше порядка знаменателя!"));
        return;
    }
    try {
        current_tf_ = tf_builder::plant(std::move(num), std::move(den), form_->delayTime(), model_param_.approxOrder);
        form_->setTransferFunction(&current_tf_);
        charts_->replaceLastFromTf(current_tf_, model_param_, form_->linkName());
        update_metrics();
    }
    catch (const std::exception& ex) {
        show_error(QString::fromUtf8(ex.what()));
    }
}

void AnalysisTab::clearCharts() {
    charts_->clearAll();
    metrics_->updateValues({});
}

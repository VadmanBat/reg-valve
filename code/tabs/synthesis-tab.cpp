#include "code/tabs/synthesis-tab.h"
#include "ui_synthesis-tab.h"

#include "code/control/regulator-factory.hpp"
#include "code/dialogs/help-dialog.h"
#include "code/dialogs/mod-par-dialog.h"
#include "code/util/tf-builder.hpp"

#include "numina/classes/control/transfer-function/response-lab.h"

#include <QCheckBox>
#include <QMenu>
#include <QMessageBox>
#include <QToolButton>
#include <QVBoxLayout>

SynthesisTab::SynthesisTab(QWidget* parent) : QWidget(parent), ui(new Ui::SynthesisTab) {
    ui->setupUi(this);
    installCustomWidgets();
    setupMetrics();

    charts_ = new ResponseChartBank(this);
    ui->chartsLayout->addWidget(charts_);

    ui->verticalLayout->setStretch(0, 0);
    ui->verticalLayout->setStretch(1, 0);
    ui->verticalLayout->setStretch(2, 0);
    ui->verticalLayout->setStretch(3, 1);

    chartsMenu_ = new QMenu(this);
    connect(chartsMenu_, &QMenu::aboutToShow, this, [this] { charts_->populateMenu(chartsMenu_); });

    auto* chartsBtn = new QToolButton(this);
    chartsBtn->setObjectName(QStringLiteral("chartsButton"));
    chartsBtn->setText(QStringLiteral("◫"));
    chartsBtn->setToolTip(tr("Отображаемые графики"));
    chartsBtn->setPopupMode(QToolButton::InstantPopup);
    chartsBtn->setMenu(chartsMenu_);
    chartsBtn->setFixedSize(40, 40);
    ui->buttonLayout->insertWidget(ui->buttonLayout->indexOf(ui->settingsButton), chartsBtn);

    form_->setTransferFunction(&currentTf_);

    connect(ui->helpButton, &QPushButton::clicked, this, &SynthesisTab::openHelp);
    connect(ui->settingsButton, &QPushButton::clicked, this, &SynthesisTab::openSettings);
    connect(ui->addButton, &QPushButton::clicked, this, &SynthesisTab::addTransferFunction);
    connect(ui->clearButton, &QPushButton::clicked, this, &SynthesisTab::clearCharts);
}

SynthesisTab::~SynthesisTab() {
    delete ui;
}

void SynthesisTab::installCustomWidgets() {
    form_ = new TranFuncForm(6, 6, QStringLiteral("W<sub>ОУ</sub>(p) = "), ui->formHost);
    auto* formLayout = new QVBoxLayout(ui->formHost);
    formLayout->setContentsMargins(0, 0, 0, 0);
    formLayout->addWidget(form_, 0, Qt::AlignLeft | Qt::AlignVCenter);

    metrics_ = new RegulationWidget(3, 4, ui->metricsHost);
    auto* metricsLayout = new QVBoxLayout(ui->metricsHost);
    metricsLayout->setContentsMargins(0, 0, 0, 0);
    metricsLayout->addWidget(metrics_, 0, Qt::AlignRight | Qt::AlignVCenter);
    if (ui->topLayout)
        ui->topLayout->insertStretch(1, 1);

    parameters_ = {
        new RegParameter(QStringLiteral("K<sub>p</sub>"), 0.05, 2000, 0.05, 5, this),
        new RegParameter(QStringLiteral("T<sub>u</sub>"), 0.05, 2000, 1, 120, this),
        new RegParameter(QStringLiteral("T<sub>d</sub>"), 0.05, 2000, 1, 60, this),
    };
    ui->paramsLayout->setSpacing(0);
    ui->paramsLayout->setContentsMargins(0, 0, 0, 0);
    ui->paramsLayout->setSizeConstraint(QLayout::SetMinimumSize);
    for (auto* p : parameters_) {
        ui->paramsLayout->addLayout(p->layout());
        connect(p->checkBox(), &QCheckBox::toggled, this, &SynthesisTab::replaceTransferFunction);
        connect(p->slider(), &DoubleSlider::doubleValueChanged, this, &SynthesisTab::replaceTransferFunction);
    }
}

void SynthesisTab::setupMetrics() {
    metrics_->setLabels({
        QStringLiteral("t<sub>р</sub>:"),  QStringLiteral("ω<sub>n</sub>:"), QStringLiteral("h<sub>уст</sub>:"),
        QStringLiteral("ЛИК:"),            QStringLiteral("t<sub>н</sub>:"),  QStringLiteral("ω<sub>c</sub>:"),
        QStringLiteral("σ<sub>ст</sub>:"), QStringLiteral("ИКК:"),           QStringLiteral("t<sub>п</sub>:"),
        QStringLiteral("ζ:"),              QStringLiteral("σ<sub>пр</sub>:"), QStringLiteral("СКО:"),
    });
    metrics_->setPrecisions({2, 4, 2, 4, 2, 4, 2, 4, 2, 4, 2, 4});
    metrics_->setColors(
        {{1, 2}, {0, 0}, {0, 0}, {1, 2}, {1, 2}, {0, 0}, {0, 0}, {1, 2}, {1, 2}, {2, 1}, {1, 2}, {1, 2}});
}

void SynthesisTab::showError(const QString& message) {
    QMessageBox::critical(this, tr("Ошибка ввода"), message);
}

void SynthesisTab::openSettings() {
    ModParDialog dialog(modelParam_, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    modelParam_ = dialog.data();
    // Rebuild last chart with new simulation/frequency params.
    if (!charts_->empty())
        applyCurrentRegulator(true);
}

void SynthesisTab::openHelp() {
    HelpDialog dialog(this);
    dialog.exec();
}

void SynthesisTab::applyCurrentRegulator(bool replaceLast) {
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

        // Honour model parameters (time/freq range) from settings dialog.
        const QString title = QString::fromStdString(reg.title);
        if (replaceLast && !charts_->empty())
            charts_->replaceLastFromTf(currentTf_, modelParam_, title);
        else
            charts_->appendFromTf(currentTf_, modelParam_, title);

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
    } catch (const std::exception& ex) {
        showError(QString::fromUtf8(ex.what()));
    }
}

void SynthesisTab::addTransferFunction() {
    applyCurrentRegulator(false);
}

void SynthesisTab::replaceTransferFunction() {
    if (charts_->empty())
        return;
    applyCurrentRegulator(true);
}

void SynthesisTab::clearCharts() {
    charts_->clearAll();
    metrics_->updateValues({});
}

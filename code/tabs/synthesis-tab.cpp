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

#include <algorithm>
#include <cmath>

SynthesisTab::SynthesisTab(QWidget* parent) : QWidget(parent), ui(new Ui::SynthesisTab) {
    ui->setupUi(this);
    install_custom_widgets();
    setup_metrics();

    charts_ = new ResponseChartBank(this);
    charts_->setTransientTitle(tr("Переходный процесс"));
    ui->chartsLayout->addWidget(charts_);

    ui->verticalLayout->setStretch(0, 0);
    ui->verticalLayout->setStretch(1, 0);
    ui->verticalLayout->setStretch(2, 0);
    ui->verticalLayout->setStretch(3, 1);

    charts_menu_ = new QMenu(this);
    connect(charts_menu_, &QMenu::aboutToShow, this, [this] { charts_->populateMenu(charts_menu_); });

    auto* charts_btn = new QToolButton(this);
    charts_btn->setObjectName(QStringLiteral("chartsButton"));
    charts_btn->setText(QStringLiteral("◫"));
    charts_btn->setToolTip(tr("Отображаемые графики"));
    charts_btn->setPopupMode(QToolButton::InstantPopup);
    charts_btn->setMenu(charts_menu_);
    charts_btn->setFixedSize(40, 40);
    ui->buttonLayout->insertWidget(ui->buttonLayout->indexOf(ui->settingsButton), charts_btn);

    form_->setTransferFunction(&current_tf_);

    connect(ui->helpButton, &QPushButton::clicked, this, &SynthesisTab::openHelp);
    connect(ui->settingsButton, &QPushButton::clicked, this, &SynthesisTab::openSettings);
    connect(ui->autoSynthButton, &QPushButton::clicked, this, &SynthesisTab::autoSynthesize);
    connect(ui->addButton, &QPushButton::clicked, this, &SynthesisTab::addTransferFunction);
    connect(ui->clearButton, &QPushButton::clicked, this, &SynthesisTab::clearCharts);
}

SynthesisTab::~SynthesisTab() {
    delete ui;
}

void SynthesisTab::install_custom_widgets() {
    form_ = new TranFuncForm(6, 6, QStringLiteral("W<sub>ОУ</sub>(p) = "), ui->formHost);
    auto* form_layout = new QVBoxLayout(ui->formHost);
    form_layout->setContentsMargins(0, 0, 0, 0);
    form_layout->addWidget(form_, 0, Qt::AlignLeft | Qt::AlignVCenter);

    metrics_ = new RegulationWidget(3, 4, ui->metricsHost);
    auto* metrics_layout = new QVBoxLayout(ui->metricsHost);
    metrics_layout->setContentsMargins(0, 0, 0, 0);
    metrics_layout->addWidget(metrics_, 0, Qt::AlignRight | Qt::AlignVCenter);
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

void SynthesisTab::setup_metrics() {
    metrics_->setLabels({
        QStringLiteral("t<sub>р</sub>:"),  QStringLiteral("ω<sub>n</sub>:"), QStringLiteral("h<sub>уст</sub>:"),
        QStringLiteral("ЛИК:"),            QStringLiteral("t<sub>н</sub>:"),  QStringLiteral("ω<sub>c</sub>:"),
        QStringLiteral("σ<sub>ст</sub>:"), QStringLiteral("ИКК:"),           QStringLiteral("t<sub>п</sub>:"),
        QStringLiteral("ζ:"),              QStringLiteral("σ<sub>пр</sub>:"), QStringLiteral("СКО:"),
    });
    metrics_->setColors(
        {{1, 2}, {0, 0}, {0, 0}, {1, 2}, {1, 2}, {0, 0}, {0, 0}, {1, 2}, {1, 2}, {2, 1}, {1, 2}, {1, 2}});
}

void SynthesisTab::show_error(const QString& message) {
    QMessageBox::critical(this, tr("Ошибка ввода"), message);
}

void SynthesisTab::openSettings() {
    ModParDialog dialog(model_param_, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    model_param_ = dialog.data();
    if (!charts_->empty())
        apply_current_regulator(true);
}

void SynthesisTab::openHelp() {
    HelpDialog dialog(this);
    dialog.exec();
}

void SynthesisTab::autoSynthesize() {
    auto plant_num = form_->numerator();
    auto plant_den = form_->denominator();
    if (!tf_builder::validInput(plant_num, plant_den)) {
        show_error(tr("Задайте корректную ПФ объекта управления."));
        return;
    }

    try {
        const auto plant =
            tf_builder::plant(std::move(plant_num), std::move(plant_den), form_->delayTime(),
                              model_param_.approxOrder);
        numina::ResponseLab lab(plant);
        const auto q = lab.evaluate();

        // Эвристика PI по DC-усилению и времени переходного процесса ОУ
        // (numina::RegulatorDesigner пока TODO).
        double K = 1.0;
        if (q.is_settled && std::abs(q.steady_state) > 1e-9)
            K = std::abs(q.steady_state);
        else {
            // fallback: free terms N(0)/D(0)
            const auto n = plant.getNumerator().vector();
            const auto d = plant.getDenominator().vector();
            if (!n.empty() && !d.empty() && std::abs(d.back()) > 1e-14)
                K = std::max(1e-6, std::abs(n.back() / d.back()));
        }

        double T = q.is_settled && q.settling_time > 1e-6 ? q.settling_time : 10.0;
        if (q.is_settled && q.rise_time > 1e-6)
            T = std::max(T, q.rise_time * 4.0);

        // Rough PI (open-loop style): Kp ~ 1/K, Tu ~ T/2
        double Kp = std::clamp(1.0 / K, 0.05, 2000.0);
        double Tu = std::clamp(T / 2.0, 0.05, 2000.0);
        double Td = std::clamp(T / 8.0, 0.05, 2000.0);

        auto set_param = [](RegParameter* p, double v, bool on) {
            const double lo = std::max(0.05, v / 20.0);
            const double hi = std::min(2000.0, std::max(v * 20.0, lo + 1.0));
            p->setRange(lo, hi);
            p->setEnabled(on);
            p->setValue(v);
        };

        set_param(parameters_[0], Kp, true);  // P
        set_param(parameters_[1], Tu, true);  // I
        set_param(parameters_[2], Td, false); // D off by default

        apply_current_regulator(!charts_->empty());
    } catch (const std::exception& ex) {
        show_error(tr("Автосинтез: %1").arg(QString::fromUtf8(ex.what())));
    }
}

void SynthesisTab::apply_current_regulator(bool replace_last) {
    auto plant_num = form_->numerator();
    auto plant_den = form_->denominator();
    if (!tf_builder::validInput(plant_num, plant_den))
        return;

    const auto reg = regulator_factory::make(parameters_[0]->enabled(), parameters_[1]->enabled(),
                                             parameters_[2]->enabled(), parameters_[0]->value(),
                                             parameters_[1]->value(), parameters_[2]->value());

    try {
        current_tf_ = tf_builder::closedLoop(std::move(plant_num), std::move(plant_den), reg.num, reg.den,
                                             form_->delayTime(), model_param_.approxOrder);
        form_->setTransferFunction(&current_tf_);

        const QString title = QString::fromStdString(reg.title);
        if (replace_last && !charts_->empty())
            charts_->replaceLastFromTf(current_tf_, model_param_, title);
        else
            charts_->appendFromTf(current_tf_, model_param_, title);

        numina::ResponseLab lab(current_tf_);
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
        show_error(QString::fromUtf8(ex.what()));
    }
}

void SynthesisTab::addTransferFunction() {
    apply_current_regulator(false);
}

void SynthesisTab::replaceTransferFunction() {
    if (charts_->empty())
        return;
    apply_current_regulator(true);
}

void SynthesisTab::clearCharts() {
    charts_->clearAll();
    metrics_->updateValues({});
}

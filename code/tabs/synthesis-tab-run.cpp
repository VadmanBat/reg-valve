#include "code/control/regulator-factory.hpp"
#include "code/dialogs/mod-par-dialog.h"
#include "code/tabs/synthesis-tab.h"
#include "code/util/tf-builder.hpp"
#include "numina/classes/control/transfer-function/response-lab.h"

#include <algorithm>
#include <cmath>
#include <QCheckBox>
#include <QTimer>

void SynthesisTab::block_param_signals(bool block) {
    for (auto* p : parameters_) {
        p->checkBox()->blockSignals(block);
        p->slider()->blockSignals(block);
    }
}

void SynthesisTab::schedule_replace() {
    if (!charts_->empty())
        recompute_timer_->start();
}

void SynthesisTab::update_metrics_from_bank() {
    if (!charts_->hasLastQuality() || !charts_->lastQuality().is_settled) {
        metrics_->updateValues({});
        return;
    }
    const auto& q = charts_->lastQuality();
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
}

void SynthesisTab::openSettings() {
    ModParDialog dialog(model_param_, this);
    if (dialog.exec() != QDialog::Accepted)
        return;
    model_param_ = dialog.data();
    if (charts_->empty())
        return;
    try {
        charts_->recomputeAll(model_param_);
        update_metrics_from_bank();
    }
    catch (const std::exception& ex) {
        show_error(QString::fromUtf8(ex.what()));
    }
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
            tf_builder::plant(std::move(plant_num), std::move(plant_den), form_->delayTime(), model_param_.approxOrder);
        numina::ResponseLab lab(plant);
        const auto q = lab.evaluate();

        // Heuristic PI from plant DC gain / settling (RegulatorDesigner TODO in numina).
        double K = 1.0;
        if (q.is_settled && std::abs(q.steady_state) > 1e-9)
            K = std::abs(q.steady_state);
        else {
            const auto n = plant.getNumerator().vector();
            const auto d = plant.getDenominator().vector();
            if (!n.empty() && !d.empty() && std::abs(d.back()) > 1e-14)
                K = std::max(1e-6, std::abs(n.back() / d.back()));
        }

        double T = q.is_settled && q.settling_time > 1e-6 ? q.settling_time : 10.0;
        if (q.is_settled && q.rise_time > 1e-6)
            T = std::max(T, q.rise_time * 4.0);

        recompute_timer_->stop();
        block_param_signals(true);
        auto set_param = [](RegParameter* p, double v, bool on) {
            const double lo = std::max(0.05, v / 20.0);
            const double hi = std::min(2000.0, std::max(v * 20.0, lo + 1.0));
            p->setRange(lo, hi);
            p->setEnabled(on);
            p->setValue(v);
        };
        set_param(parameters_[0], std::clamp(1.0 / K, 0.05, 2000.0), true);
        set_param(parameters_[1], std::clamp(T / 2.0, 0.05, 2000.0), true);
        set_param(parameters_[2], std::clamp(T / 8.0, 0.05, 2000.0), false);
        block_param_signals(false);

        apply_current_regulator(!charts_->empty());
    }
    catch (const std::exception& ex) {
        show_error(tr("Автосинтез: %1").arg(QString::fromUtf8(ex.what())));
    }
}

void SynthesisTab::apply_current_regulator(bool replace_last) {
    auto plant_num = form_->numerator();
    auto plant_den = form_->denominator();
    if (!tf_builder::validInput(plant_num, plant_den))
        return;

    const auto reg =
        regulator_factory::make(parameters_[0]->enabled(), parameters_[1]->enabled(), parameters_[2]->enabled(),
                                parameters_[0]->value(), parameters_[1]->value(), parameters_[2]->value());

    try {
        const double tau = form_->delayTime();
        const int order  = model_param_.approxOrder;
        plant_tf_        = tf_builder::plant(plant_num, plant_den, tau, order);
        current_tf_ = tf_builder::closedLoop(std::move(plant_num), std::move(plant_den), reg.num, reg.den, tau, order);
        form_->setTransferFunction(&plant_tf_);

        const QString title = QString::fromStdString(reg.title);
        if (replace_last && !charts_->empty())
            charts_->replaceLastFromTf(current_tf_, model_param_, title);
        else
            charts_->appendFromTf(current_tf_, model_param_, title);

        update_metrics_from_bank();
    }
    catch (const std::exception& ex) {
        show_error(QString::fromUtf8(ex.what()));
    }
}

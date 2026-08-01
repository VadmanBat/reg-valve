#include "code/charts/response-chart-bank.h"

void ResponseChartBank::refit_all() {
    if (!tran_bounds_.empty())
        chart_tran_->fitAxes(tran_bounds_.min_x(), tran_bounds_.max_x(), tran_bounds_.min_y(), tran_bounds_.max_y());
    if (!impulse_bounds_.empty())
        chart_impulse_->fitAxes(impulse_bounds_.min_x(), impulse_bounds_.max_x(), impulse_bounds_.min_y(),
                                impulse_bounds_.max_y());
    if (!nyquist_bounds_.empty())
        chart_nyquist_->fitAxes(nyquist_bounds_.min_x(), nyquist_bounds_.max_x(), nyquist_bounds_.min_y(),
                                nyquist_bounds_.max_y());
    if (!amp_bounds_.empty())
        chart_amp_->fitAxes(amp_bounds_.min_x(), amp_bounds_.max_x(), amp_bounds_.min_y(), amp_bounds_.max_y());
    if (!phase_bounds_.empty())
        chart_phase_->fitAxes(phase_bounds_.min_x(), phase_bounds_.max_x(), phase_bounds_.min_y(),
                              phase_bounds_.max_y());
}

ResponseChartBank::Batch ResponseChartBank::compute_batch(const numina::TransferFunction& tf, const ModelParam& params,
                                                          const QString& name) {
    Batch b;
    b.name          = name;
    b.transient     = tf_builder::transient(tf, params);
    b.impulse       = tf_builder::impulse(tf, params);
    const auto freq = tf_builder::frequencyBundle(tf, params);
    b.nyquist       = std::move(freq.nyquist);
    b.amplitude     = std::move(freq.amplitude);
    b.phase         = std::move(freq.phase);
    return b;
}

void ResponseChartBank::push_batch(Batch b, bool replace_last) {
    if (replace_last && !history_.empty()) {
        history_.pop_back();
        if (!tran_bounds_.empty())
            tran_bounds_.pop_back();
        if (!impulse_bounds_.empty())
            impulse_bounds_.pop_back();
        if (!nyquist_bounds_.empty())
            nyquist_bounds_.pop_back();
        if (!amp_bounds_.empty())
            amp_bounds_.pop_back();
        if (!phase_bounds_.empty())
            phase_bounds_.pop_back();
    }

    auto push_real = [](ChartPanel* panel, BoundsSet& bounds, const tf_builder::VecPair& data, const QString& name,
                        bool replace) {
        if (replace && panel->curveCount() > 0)
            panel->replaceLastRealCurve(data, name);
        else
            panel->addRealCurve(data, name);
        bounds.push_back(boundsOfReal(data));
    };
    auto push_complex = [](ChartPanel* panel, BoundsSet& bounds, const tf_builder::VecComp& data, const QString& name,
                           bool replace) {
        if (replace && panel->curveCount() > 0)
            panel->replaceLastComplexCurve(data, name);
        else
            panel->addComplexCurve(data, name);
        bounds.push_back(boundsOfComplex(data));
    };

    push_real(chart_tran_, tran_bounds_, b.transient, b.name, replace_last);
    push_real(chart_impulse_, impulse_bounds_, b.impulse, b.name, replace_last);
    push_complex(chart_nyquist_, nyquist_bounds_, b.nyquist, b.name, replace_last);
    push_real(chart_amp_, amp_bounds_, b.amplitude, b.name, replace_last);
    push_real(chart_phase_, phase_bounds_, b.phase, b.name, replace_last);

    history_.push_back(std::move(b));
    refit_all();
}

void ResponseChartBank::appendFromTf(const numina::TransferFunction& tf, const ModelParam& params,
                                     const QString& name) {
    push_batch(compute_batch(tf, params, name), false);
}

void ResponseChartBank::appendTransientCurve(const chart_utils::VecPair& points, const QString& name) {
    if (points.empty())
        return;
    chart_tran_->addRealCurve(points, name);
    tran_bounds_.push_back(boundsOfReal(points));
    chart_tran_->fitAxes(tran_bounds_.min_x(), tran_bounds_.max_x(), tran_bounds_.min_y(), tran_bounds_.max_y());
}

void ResponseChartBank::replaceLastFromTf(const numina::TransferFunction& tf, const ModelParam& params,
                                          const QString& name) {
    if (history_.empty()) {
        appendFromTf(tf, params, name);
        return;
    }
    push_batch(compute_batch(tf, params, name), true);
}

void ResponseChartBank::clearAll() {
    history_.clear();
    chart_tran_->clearCurves();
    chart_impulse_->clearCurves();
    chart_nyquist_->clearCurves();
    chart_amp_->clearCurves();
    chart_phase_->clearCurves();
    tran_bounds_.clear();
    impulse_bounds_.clear();
    nyquist_bounds_.clear();
    amp_bounds_.clear();
    phase_bounds_.clear();
}

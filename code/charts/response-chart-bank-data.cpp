#include "code/charts/response-chart-bank.h"

#include "numina/classes/control/transfer-function/response-lab.h"

namespace {

void set_panel_updates(ChartPanel* panel, bool on) {
    if (panel && panel->view()) {
        panel->view()->setUpdatesEnabled(on);
        if (on)
            panel->view()->viewport()->update();
    }
}

} // namespace

void ResponseChartBank::refit_all() {
    constexpr bool kNiceX_t_w = false;
    constexpr bool kNiceY     = true;
    constexpr bool kNiceBoth  = true;

    if (vis_.transient && !tran_bounds_.empty())
        chart_tran_->fitAxes(tran_bounds_.min_x(), tran_bounds_.max_x(), tran_bounds_.min_y(), tran_bounds_.max_y(),
                             kNiceX_t_w, kNiceY);
    if (vis_.impulse && !impulse_bounds_.empty())
        chart_impulse_->fitAxes(impulse_bounds_.min_x(), impulse_bounds_.max_x(), impulse_bounds_.min_y(),
                                impulse_bounds_.max_y(), kNiceX_t_w, kNiceY);
    if (vis_.nyquist && !nyquist_bounds_.empty())
        chart_nyquist_->fitAxes(nyquist_bounds_.min_x(), nyquist_bounds_.max_x(), nyquist_bounds_.min_y(),
                                nyquist_bounds_.max_y(), kNiceBoth, kNiceBoth);
    if (vis_.amplitude && !amp_bounds_.empty())
        chart_amp_->fitAxes(amp_bounds_.min_x(), amp_bounds_.max_x(), amp_bounds_.min_y(), amp_bounds_.max_y(),
                            kNiceX_t_w, kNiceY);
    if (vis_.phase && !phase_bounds_.empty())
        chart_phase_->fitAxes(phase_bounds_.min_x(), phase_bounds_.max_x(), phase_bounds_.min_y(),
                              phase_bounds_.max_y(), kNiceX_t_w, kNiceY);
}

void ResponseChartBank::ensure_visible_series() {
    const auto n_hist = history_.size();
    if (n_hist == 0)
        return;

    // Materialize series only if panel was hidden during adds. curveCount > n_hist: overlays (Id experiment).
    auto materialize = [&]<typename AddFn>(ChartPanel* panel, BoundsSet& bounds, bool visible, auto getter,
                                           AddFn add) {
        if (!visible || !panel || panel->curveCount() >= n_hist)
            return;

        set_panel_updates(panel, false);
        if (panel->curveCount() == 0) {
            // Bounds already tracked while panel was hidden — only build QLineSeries.
            for (const Batch& batch : history_)
                add(panel, getter(batch), batch.name);
        }
        else {
            // Partial series (e.g. after clearDataSeries path): rebuild series + bounds in one pass each.
            panel->clearDataSeries();
            bounds.clear();
            for (const Batch& batch : history_)
                bounds.push_back(add(panel, getter(batch), batch.name));
        }
        set_panel_updates(panel, true);
    };

    const auto add_real = [](ChartPanel* p, const tf_builder::VecPair& d, const QString& n) {
        return p->addRealCurve(d, n);
    };
    const auto add_cx = [](ChartPanel* p, const tf_builder::VecComp& d, const QString& n) {
        return p->addComplexCurve(d, n);
    };

    materialize(chart_tran_, tran_bounds_, vis_.transient, [](const Batch& b) -> const auto& { return b.transient; },
                add_real);
    materialize(chart_impulse_, impulse_bounds_, vis_.impulse, [](const Batch& b) -> const auto& { return b.impulse; },
                add_real);
    materialize(chart_nyquist_, nyquist_bounds_, vis_.nyquist, [](const Batch& b) -> const auto& { return b.nyquist; },
                add_cx);
    materialize(chart_amp_, amp_bounds_, vis_.amplitude, [](const Batch& b) -> const auto& { return b.amplitude; },
                add_real);
    materialize(chart_phase_, phase_bounds_, vis_.phase, [](const Batch& b) -> const auto& { return b.phase; },
                add_real);
}

ResponseChartBank::Batch ResponseChartBank::compute_batch(const numina::TransferFunction& tf, const ModelParam& params,
                                                          const QString& name) {
    Batch b;
    b.name = name;
    numina::ResponseLab lab(tf);
    b.transient     = tf_builder::transient(lab, params);
    b.impulse       = tf_builder::impulse(lab, params);
    const auto freq = tf_builder::frequencyBundle(lab, params);
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

    ChartPanel* panels[] = {chart_tran_, chart_impulse_, chart_nyquist_, chart_amp_, chart_phase_};
    const bool show[]    = {vis_.transient, vis_.impulse, vis_.nyquist, vis_.amplitude, vis_.phase};
    for (int i = 0; i < 5; ++i) {
        if (show[i])
            set_panel_updates(panels[i], false);
    }

    // One BoundsSet entry per history batch (aligned with replace_last pop), even if data empty.
    auto push_real = [](bool visible, ChartPanel* panel, BoundsSet& bounds, const tf_builder::VecPair& data,
                        const QString& name, bool replace) {
        if (visible && panel) {
            // Visible: one O(n) pass → QLineSeries + extents (empty data → default bounds, no series).
            if (replace && panel->curveCount() > 0)
                bounds.push_back(panel->replaceLastRealCurve(data, name));
            else
                bounds.push_back(panel->addRealCurve(data, name));
            return;
        }
        // Hidden: extents only (series from history_ when panel is shown).
        bounds.push_back(boundsOfReal(data));
    };
    auto push_complex = [](bool visible, ChartPanel* panel, BoundsSet& bounds, const tf_builder::VecComp& data,
                           const QString& name, bool replace) {
        if (visible && panel) {
            if (replace && panel->curveCount() > 0)
                bounds.push_back(panel->replaceLastComplexCurve(data, name));
            else
                bounds.push_back(panel->addComplexCurve(data, name));
            return;
        }
        bounds.push_back(boundsOfComplex(data));
    };

    push_real(vis_.transient, chart_tran_, tran_bounds_, b.transient, b.name, replace_last);
    push_real(vis_.impulse, chart_impulse_, impulse_bounds_, b.impulse, b.name, replace_last);
    push_complex(vis_.nyquist, chart_nyquist_, nyquist_bounds_, b.nyquist, b.name, replace_last);
    push_real(vis_.amplitude, chart_amp_, amp_bounds_, b.amplitude, b.name, replace_last);
    push_real(vis_.phase, chart_phase_, phase_bounds_, b.phase, b.name, replace_last);

    history_.push_back(std::move(b));
    refit_all();

    for (int i = 0; i < 5; ++i) {
        if (show[i])
            set_panel_updates(panels[i], true);
    }
}

void ResponseChartBank::appendFromTf(const numina::TransferFunction& tf, const ModelParam& params,
                                     const QString& name) {
    push_batch(compute_batch(tf, params, name), false);
}

void ResponseChartBank::appendTransientCurve(const chart_utils::VecPair& points, const QString& name) {
    if (points.empty())
        return;
    tran_bounds_.push_back(chart_tran_->addRealCurve(points, name));
    if (vis_.transient)
        chart_tran_->fitAxes(tran_bounds_.min_x(), tran_bounds_.max_x(), tran_bounds_.min_y(), tran_bounds_.max_y(),
                             /*niceX=*/false, /*niceY=*/true);
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

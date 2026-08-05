#include "code/charts/response-chart-bank.h"
#include "numina/classes/control/transfer-function/response-lab.h"

#include <algorithm>

namespace {

void set_panel_updates(ChartPanel* panel, bool on) {
    if (panel && panel->view()) {
        panel->view()->setUpdatesEnabled(on);
        if (on)
            panel->view()->viewport()->update();
    }
}

void fit_panel(ChartPanel* panel, double min_x, double max_x, double min_y, double max_y, bool nice_x, bool nice_y) {
    if (!panel)
        return;
    panel->fitAxes(min_x, max_x, min_y, max_y, nice_x, nice_y);
}

void merge_bounds(double& min_x, double& max_x, double& min_y, double& max_y, const AxisBounds& b) {
    min_x = std::min(min_x, b.min_x);
    max_x = std::max(max_x, b.max_x);
    min_y = std::min(min_y, b.min_y);
    max_y = std::max(max_y, b.max_y);
}

} // namespace

void ResponseChartBank::fit_transient() {
    if (!vis_.transient || !chart_tran_)
        return;

    const bool has_hist = !tran_bounds_.empty();
    if (!has_hist && !has_tran_overlay_)
        return;

    double min_x = 0.0, max_x = 0.0, min_y = 0.0, max_y = 0.0;
    if (has_hist) {
        min_x = tran_bounds_.min_x();
        max_x = tran_bounds_.max_x();
        min_y = tran_bounds_.min_y();
        max_y = tran_bounds_.max_y();
        if (has_tran_overlay_)
            merge_bounds(min_x, max_x, min_y, max_y, tran_overlay_bounds_);
    }
    else {
        min_x = tran_overlay_bounds_.min_x;
        max_x = tran_overlay_bounds_.max_x;
        min_y = tran_overlay_bounds_.min_y;
        max_y = tran_overlay_bounds_.max_y;
    }
    fit_panel(chart_tran_, min_x, max_x, min_y, max_y, /*niceX=*/false, /*niceY=*/true);
}

void ResponseChartBank::refit_all() {
    constexpr bool kNiceX_t_w = false;
    constexpr bool kNiceY     = true;
    constexpr bool kNiceBoth  = true;

    fit_transient();
    if (vis_.impulse && !impulse_bounds_.empty())
        fit_panel(chart_impulse_, impulse_bounds_.min_x(), impulse_bounds_.max_x(), impulse_bounds_.min_y(),
                  impulse_bounds_.max_y(), kNiceX_t_w, kNiceY);
    if (vis_.nyquist && !nyquist_bounds_.empty())
        fit_panel(chart_nyquist_, nyquist_bounds_.min_x(), nyquist_bounds_.max_x(), nyquist_bounds_.min_y(),
                  nyquist_bounds_.max_y(), kNiceBoth, kNiceBoth);
    if (vis_.amplitude && !amp_bounds_.empty())
        fit_panel(chart_amp_, amp_bounds_.min_x(), amp_bounds_.max_x(), amp_bounds_.min_y(), amp_bounds_.max_y(),
                  kNiceX_t_w, kNiceY);
    if (vis_.phase && !phase_bounds_.empty())
        fit_panel(chart_phase_, phase_bounds_.min_x(), phase_bounds_.max_x(), phase_bounds_.min_y(),
                  phase_bounds_.max_y(), kNiceX_t_w, kNiceY);
}

void ResponseChartBank::ensure_visible_series() {
    const auto n_hist = history_.size();

    auto materialize = [&]<typename AddFn>(ChartPanel* panel, BoundsSet& bounds, bool visible, auto getter, AddFn add,
                                           bool with_overlay) {
        if (!visible || !panel)
            return;

        const std::size_t expected = n_hist + (with_overlay && has_tran_overlay_ ? 1u : 0u);
        if (expected == 0 || panel->curveCount() >= expected)
            return;

        set_panel_updates(panel, false);
        panel->clearDataSeries();
        // Rebuild series only; BoundsSet already tracks extents (overlay is separate).
        if (with_overlay && has_tran_overlay_)
            (void)panel->addRealCurve(tran_overlay_points_, tran_overlay_name_);
        for (const Batch& batch : history_)
            (void)add(panel, getter(batch), batch.name);
        (void)bounds;
        set_panel_updates(panel, true);
    };

    const auto add_real = [](ChartPanel* p, const tf_builder::VecPair& d, const QString& n) {
        return p->addRealCurve(d, n);
    };
    const auto add_cx = [](ChartPanel* p, const tf_builder::VecComp& d, const QString& n) {
        return p->addComplexCurve(d, n);
    };

    materialize(
        chart_tran_, tran_bounds_, vis_.transient, [](const Batch& b) -> const auto& { return b.transient; }, add_real,
        /*with_overlay=*/true);
    materialize(
        chart_impulse_, impulse_bounds_, vis_.impulse, [](const Batch& b) -> const auto& { return b.impulse; },
        add_real, false);
    materialize(
        chart_nyquist_, nyquist_bounds_, vis_.nyquist, [](const Batch& b) -> const auto& { return b.nyquist; }, add_cx,
        false);
    materialize(
        chart_amp_, amp_bounds_, vis_.amplitude, [](const Batch& b) -> const auto& { return b.amplitude; }, add_real,
        false);
    materialize(
        chart_phase_, phase_bounds_, vis_.phase, [](const Batch& b) -> const auto& { return b.phase; }, add_real, false);
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
    // Overlay series are never the "last" history curve: replace only when curveCount > overlay.
    auto history_series_count = [this](ChartPanel* panel, bool is_tran) -> std::size_t {
        if (!panel)
            return 0;
        const std::size_t n = panel->curveCount();
        if (is_tran && has_tran_overlay_ && n > 0)
            return n - 1;
        return n;
    };

    auto push_real = [&](bool visible, ChartPanel* panel, BoundsSet& bounds, const tf_builder::VecPair& data,
                         const QString& name, bool replace, bool is_tran) {
        if (visible && panel) {
            if (replace && history_series_count(panel, is_tran) > 0)
                bounds.push_back(panel->replaceLastRealCurve(data, name));
            else
                bounds.push_back(panel->addRealCurve(data, name));
            return;
        }
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

    push_real(vis_.transient, chart_tran_, tran_bounds_, b.transient, b.name, replace_last, true);
    push_real(vis_.impulse, chart_impulse_, impulse_bounds_, b.impulse, b.name, replace_last, false);
    push_complex(vis_.nyquist, chart_nyquist_, nyquist_bounds_, b.nyquist, b.name, replace_last);
    push_real(vis_.amplitude, chart_amp_, amp_bounds_, b.amplitude, b.name, replace_last, false);
    push_real(vis_.phase, chart_phase_, phase_bounds_, b.phase, b.name, replace_last, false);

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

    has_tran_overlay_     = true;
    tran_overlay_name_    = name;
    tran_overlay_points_  = points;
    tran_overlay_bounds_  = boundsOfReal(points);

    // Only draw when visible; otherwise materialize on show (with history_).
    if (vis_.transient && chart_tran_)
        (void)chart_tran_->addRealCurve(points, name);

    fit_transient();
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
    has_tran_overlay_ = false;
    tran_overlay_name_.clear();
    tran_overlay_points_.clear();
    tran_overlay_bounds_ = {};

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

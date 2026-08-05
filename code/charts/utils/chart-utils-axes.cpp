#include "code/charts/utils/chart-utils-detail.hpp"
#include "code/charts/utils/chart-utils.hpp"

#include <cmath>
#include <QLegend>
#include <QLegendMarker>
#include <QLineSeries>
#include <QSignalBlocker>
#include <QValueAxis>

namespace chart_utils {
namespace {

QLineSeries* find_series_by_name(QChart* chart, const char* name) {
    if (!chart)
        return nullptr;
    for (auto* s : chart->series()) {
        if (s->name() == QLatin1String(name))
            return qobject_cast<QLineSeries*>(s);
    }
    return nullptr;
}

void hide_legend_marker(QChart* chart, QAbstractSeries* series) {
    if (!chart || !series || !chart->legend())
        return;
    for (auto* marker : chart->legend()->markers(series))
        marker->setVisible(false);
}

QPen guide_pen() {
    return guidePen(currentTheme());
}

void attach_all_series(QChart* chart, QValueAxis* axis_x, QValueAxis* axis_y) {
    if (!chart || !axis_x || !axis_y)
        return;
    for (auto* series : chart->series()) {
        if (!series->attachedAxes().contains(axis_x))
            series->attachAxis(axis_x);
        if (!series->attachedAxes().contains(axis_y))
            series->attachAxis(axis_y);
    }
}

void ensure_minor_ticks(QValueAxis* axis) {
    if (!axis)
        return;
    if (axis->minorTickCount() != 1)
        axis->setMinorTickCount(1);
}

/// Tab charts always show grid. Viewer may hide it — do not use there.
void force_grid_on(QValueAxis* axis) {
    if (!axis)
        return;
    if (!axis->isGridLineVisible())
        axis->setGridLineVisible(true);
    if (!axis->isMinorGridLineVisible())
        axis->setMinorGridLineVisible(true);
    ensure_minor_ticks(axis);
}

void apply_tab_grid_exact(QValueAxis* axis) {
    if (!axis)
        return;
    if (axis->tickType() != QValueAxis::TicksFixed)
        axis->setTickType(QValueAxis::TicksFixed);
    if (axis->tickCount() != kMajorTicks)
        axis->setTickCount(kMajorTicks);
    force_grid_on(axis);
}

/// Fixed ticks on a 1–2–5 lattice through 0 (…,−2s,−s,0,s,2s,…) so grid crosses origin.
/// Minimal expansion; ~kMajorTicks labels preferred.
void apply_tab_grid_snap(QValueAxis* axis) {
    if (!axis)
        return;

    const double data_lo = axis->min();
    const double data_hi = axis->max();
    if (!std::isfinite(data_lo) || !std::isfinite(data_hi) || !(data_hi > data_lo))
        return;

    const double span     = data_hi - data_lo;
    const bool nonneg     = data_lo >= 0.0; // pin lo → 0 (h(t), |W|, …)
    const bool nonpos     = data_hi <= 0.0; // pin hi → 0
    const bool cross_zero = data_lo < 0.0 && data_hi > 0.0;

    double best_lo   = data_lo;
    double best_hi   = data_hi;
    int best_count   = kMajorTicks;
    double best_cost = 1e300;
    const int target = kMajorTicks > 1 ? kMajorTicks - 1 : 1;

    for (int intervals = 3; intervals <= 8; ++intervals) {
        double step = niceNumber(span / static_cast<double>(intervals), /*round=*/true);
        if (!(step > 0.0))
            continue;

        double lo = data_lo;
        double hi = data_hi;
        if (cross_zero) {
            const int n_neg = std::max(1, static_cast<int>(std::ceil((-data_lo) / step - 1e-15)));
            const int n_pos = std::max(1, static_cast<int>(std::ceil(data_hi / step - 1e-15)));
            lo              = -static_cast<double>(n_neg) * step;
            hi              = static_cast<double>(n_pos) * step;
        }
        else if (nonneg) {
            lo              = 0.0;
            const int n_pos = std::max(1, static_cast<int>(std::ceil(data_hi / step - 1e-15)));
            hi              = static_cast<double>(n_pos) * step;
        }
        else if (nonpos) {
            hi              = 0.0;
            const int n_neg = std::max(1, static_cast<int>(std::ceil((-data_lo) / step - 1e-15)));
            lo              = -static_cast<double>(n_neg) * step;
        }
        else {
            lo = detail::snap_tick(std::floor(data_lo / step) * step, step);
            hi = detail::snap_tick(std::ceil(data_hi / step) * step, step);
        }

        lo = detail::snap_tick(lo, step);
        hi = detail::snap_tick(hi, step);
        if (!(hi > lo))
            hi = lo + step;

        const int n = static_cast<int>(std::lround((hi - lo) / step));
        if (n < 1)
            continue;
        hi = lo + static_cast<double>(n) * step;

        const double expand = (hi - lo) - span;
        const double cost   = expand * 1000.0 + std::abs(n - target);
        if (cost < best_cost) {
            best_cost  = cost;
            best_lo    = lo;
            best_hi    = hi;
            best_count = n + 1;
        }
    }

    axis->setTickType(QValueAxis::TicksFixed);
    axis->setTickCount(best_count);
    axis->setRange(best_lo, best_hi);
    force_grid_on(axis);
}

/// Viewer: refresh 1–2–5 tick step after pan/zoom. Does not force grid lines on
/// (user may have hidden them via toolbar).
void apply_viewer_grid(QValueAxis* axis) {
    if (!axis)
        return;
    const double step = niceTickStep(axis->max() - axis->min(), kMajorTicks);
    if (axis->tickType() != QValueAxis::TicksDynamic)
        axis->setTickType(QValueAxis::TicksDynamic);
    if (axis->tickAnchor() != 0.0)
        axis->setTickAnchor(0.0);
    if (std::abs(axis->tickInterval() - step) > 1e-12 * std::max(1.0, step))
        axis->setTickInterval(step);
    ensure_minor_ticks(axis);
}

void apply_axis_style(QValueAxis* axis, GridMode mode, bool snap) {
    if (mode == GridMode::Viewer)
        apply_viewer_grid(axis);
    else if (snap)
        apply_tab_grid_snap(axis);
    else
        apply_tab_grid_exact(axis);
}

} // namespace

void createAxes(QChart* chart, const QString& titleX, const QString& titleY) {
    auto* axis_x = new QValueAxis(chart);
    auto* axis_y = new QValueAxis(chart);
    axis_x->setTitleText(titleX);
    axis_y->setTitleText(titleY);
    apply_tab_grid_exact(axis_x);
    apply_tab_grid_exact(axis_y);
    const ChartTheme theme = currentTheme();
    applyAxisTheme(axis_x, theme);
    applyAxisTheme(axis_y, theme);
    chart->addAxis(axis_x, Qt::AlignBottom);
    chart->addAxis(axis_y, Qt::AlignLeft);
}

void updateOriginGuides(QChart* chart, const Pair& range_x, const Pair& range_y) {
    if (!chart)
        return;

    auto ensure = [chart](const char* name) -> QLineSeries* {
        auto* s = find_series_by_name(chart, name);
        if (s)
            return s;
        s = new QLineSeries;
        s->setName(QString::fromLatin1(name));
        s->setPen(guide_pen());
        s->setPointsVisible(false);
        chart->addSeries(s);
        detail::attachToAxes(chart, s);
        hide_legend_marker(chart, s);
        return s;
    };

    auto* hor = ensure(kHorGuide);
    auto* ver = ensure(kVerGuide);

    const bool hor_ok = hor->count() == 2 && hor->at(0).x() == range_x.first && hor->at(1).x() == range_x.second &&
                        hor->at(0).y() == 0.0 && hor->at(1).y() == 0.0;
    const bool ver_ok = ver->count() == 2 && ver->at(0).y() == range_y.first && ver->at(1).y() == range_y.second &&
                        ver->at(0).x() == 0.0 && ver->at(1).x() == 0.0;
    if (!hor_ok)
        hor->replace(QList<QPointF>{{range_x.first, 0.0}, {range_x.second, 0.0}});
    if (!ver_ok)
        ver->replace(QList<QPointF>{{0.0, range_y.first}, {0.0, range_y.second}});

    // Refresh guide color when theme flips (series may already exist).
    const QPen gpen = guide_pen();
    hor->setPen(gpen);
    ver->setPen(gpen);

    detail::attachToAxes(chart, hor);
    detail::attachToAxes(chart, ver);
    hide_legend_marker(chart, hor);
    hide_legend_marker(chart, ver);
}

void removeAllSeries(QChart* chart) {
    if (!chart)
        return;
    const auto all = chart->series();
    for (auto* series : all) {
        if (detail::isGuideSeries(series->name()))
            continue;
        chart->removeSeries(series);
        delete series;
    }
}

void applyViewerGrid(QValueAxis* axis) {
    apply_viewer_grid(axis);
}

void applyViewerGrid(QChart* chart) {
    if (!chart)
        return;
    apply_viewer_grid(qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0, nullptr)));
    apply_viewer_grid(qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).value(0, nullptr)));
}

void updateAxes(QChart* chart, const Pair& range_x, const Pair& range_y, GridMode mode, bool snap_x, bool snap_y) {
    if (!chart)
        return;
    auto* axis_x = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0, nullptr));
    auto* axis_y = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).value(0, nullptr));
    if (!axis_x || !axis_y)
        return;

    {
        const QSignalBlocker bx(axis_x);
        const QSignalBlocker by(axis_y);
        if (axis_x->min() != range_x.first || axis_x->max() != range_x.second)
            axis_x->setRange(range_x.first, range_x.second);
        if (axis_y->min() != range_y.first || axis_y->max() != range_y.second)
            axis_y->setRange(range_y.first, range_y.second);
        apply_axis_style(axis_x, mode, snap_x);
        apply_axis_style(axis_y, mode, snap_y);
        // Restyle after tick config (grid pens must stay theme-colored).
        const ChartTheme theme = currentTheme();
        applyAxisTheme(axis_x, theme);
        applyAxisTheme(axis_y, theme);
    }

    updateOriginGuides(chart, {axis_x->min(), axis_x->max()}, {axis_y->min(), axis_y->max()});
    attach_all_series(chart, axis_x, axis_y);
}

} // namespace chart_utils

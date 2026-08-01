#pragma once

#include <algorithm>
#include <cmath>
#include <utility>

namespace chart_utils {

/// Heckbert «Nice Numbers for Graph Labels» (Graphics Gems, 1990).
/// 1 / 2 / 5 × 10^k.
/// round=true → nearest; false → ceiling (для оценки span).
[[nodiscard]] inline double niceNumber(double value, bool round) noexcept {
    if (!std::isfinite(value) || value == 0.0)
        return 0.0;
    const double ax   = std::abs(value);
    const double exp  = std::floor(std::log10(ax));
    const double base = std::pow(10.0, exp);
    const double f    = ax / base; // [1, 10)

    double nf = 10.0;
    if (round) {
        if (f < 1.5)
            nf = 1.0;
        else if (f < 3.0)
            nf = 2.0;
        else if (f < 7.0)
            nf = 5.0;
        else
            nf = 10.0;
    }
    else {
        if (f <= 1.0)
            nf = 1.0;
        else if (f <= 2.0)
            nf = 2.0;
        else if (f <= 5.0)
            nf = 5.0;
        else
            nf = 10.0;
    }
    return nf * base;
}

namespace detail {

[[nodiscard]] inline double snap_tick(double v, double step) noexcept {
    if (!std::isfinite(v))
        return 0.0;
    if (std::abs(v) < 1e-12 * std::max(1.0, step))
        return 0.0;
    return v;
}

} // namespace detail

/// Expand [min,max] outward to nice tick multiples; optionally force-include 0.
/// Example: [0, 155.456] → [0, 160] (step 20).
[[nodiscard]] inline std::pair<double, double> niceAxisRange(double min_v, double max_v,
                                                             bool include_zero = true) noexcept {
    if (!std::isfinite(min_v) || !std::isfinite(max_v))
        return {-1.0, 1.0};

    const double data_min = min_v;
    const double data_max = max_v;

    if (include_zero) {
        min_v = std::min(min_v, 0.0);
        max_v = std::max(max_v, 0.0);
    }

    if (!(max_v > min_v)) {
        const double c = min_v;
        const double a = std::max(1.0, std::abs(c) * 0.1);
        min_v          = c - a;
        max_v          = c + a;
        if (include_zero) {
            min_v = std::min(min_v, 0.0);
            max_v = std::max(max_v, 0.0);
        }
    }

    // ~2% margin so the curve does not sit on the frame (one-sided if data is one-sided)
    const double span0 = max_v - min_v;
    const double pad   = 0.02 * span0;
    if (!(include_zero && data_min >= 0.0))
        min_v -= pad;
    if (!(include_zero && data_max <= 0.0))
        max_v += pad;
    if (include_zero) {
        min_v = std::min(min_v, 0.0);
        max_v = std::max(max_v, 0.0);
    }

    const double span = max_v - min_v;
    double step       = niceNumber(span / 6.0, /*round=*/true);
    if (!(step > 0.0))
        step = 1.0;

    double lo = std::floor(min_v / step) * step;
    double hi = std::ceil(max_v / step) * step;
    lo        = detail::snap_tick(lo, step);
    hi        = detail::snap_tick(hi, step);

    if (include_zero) {
        lo = std::min(lo, 0.0);
        hi = std::max(hi, 0.0);
        // Time / magnitude axes: data starts at 0 → do not open a negative half-plane
        if (data_min >= 0.0)
            lo = 0.0;
        if (data_max <= 0.0)
            hi = 0.0;
    }
    if (!(hi > lo))
        hi = lo + step;

    return {lo, hi};
}

} // namespace chart_utils

#pragma once

#include "numina/classes/control/transfer-function.h"
#include "numina/classes/control/transfer-function/response-lab.h"
#include "numina/classes/polynomial/polynomial.h"
#include "numina/core/space.hpp"

#include "code/model/model-param.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>
#include <utility>
#include <vector>

namespace tf_builder {

using VecPair = numina::ResponseLab::VecPair;
using VecComp = numina::ResponseLab::VecComp;

/// One frequency sweep shared by КЧХ / АЧХ / ФЧХ.
struct FrequencyBundle {
    VecComp nyquist;   ///< W(jω) on complex plane
    VecPair amplitude; ///< (ω, |W(jω)|)
    VecPair phase;     ///< (ω, arg W in degrees)
};

inline numina::Polynomial poly(std::vector<double> coeffs) {
    if (coeffs.empty())
        coeffs = {0.0};
    return numina::Polynomial(std::move(coeffs));
}

inline numina::TransferFunction plant(std::vector<double> num, std::vector<double> den, double tau = 0.0,
                                      int order = 6) {
    if (tau == 0.0)
        return numina::TransferFunction(poly(std::move(num)), poly(std::move(den)));
    return numina::TransferFunction{
        numina::TransferFunction(poly(std::move(num)), poly(std::move(den))),
        numina::TransferFunction::makeDelay(tau, static_cast<std::uint8_t>(order)),
    };
}

inline numina::TransferFunction closedLoop(std::vector<double> plantNum, std::vector<double> plantDen,
                                           std::vector<double> regNum, std::vector<double> regDen, double tau = 0.0,
                                           int order = 6) {
    numina::TransferFunction open{
        numina::TransferFunction(poly(std::move(plantNum)), poly(std::move(plantDen))),
        numina::TransferFunction(poly(std::move(regNum)), poly(std::move(regDen))),
    };
    if (tau != 0.0)
        open *= numina::TransferFunction::makeDelay(tau, static_cast<std::uint8_t>(order));
    open.closeLoop();
    return open;
}

inline VecPair transient(const numina::TransferFunction& tf, const ModelParam& p) {
    numina::ResponseLab lab(tf);
    if (p.autoSimTime)
        return lab.transient();
    if (p.autoTimeIntervals)
        return lab.transient({0.0, static_cast<double>(p.simTime)});
    return lab.transient({0.0, static_cast<double>(p.simTime)}, static_cast<std::size_t>(p.timeIntervals));
}

inline VecPair impulse(const numina::TransferFunction& tf, const ModelParam& p) {
    numina::ResponseLab lab(tf);
    if (p.autoSimTime)
        return lab.impulse();
    if (p.autoTimeIntervals)
        return lab.impulse({0.0, static_cast<double>(p.simTime)});
    return lab.impulse({0.0, static_cast<double>(p.simTime)}, static_cast<std::size_t>(p.timeIntervals));
}

/// True when free term of denominator is ~0 (pole at s=0 / free integrator) — avoid ω=0.
inline bool hasZeroDenConstant(const numina::TransferFunction& tf) noexcept {
    const auto& den = tf.getDenominator();
    // high→low: last coeff is free term a0
    if (den.degree() < 0)
        return true;
    const auto v = den.vector();
    if (v.empty())
        return true;
    return std::abs(v.back()) <= 1e-14 * (1.0 + std::abs(v.front()));
}

/// КЧХ + АЧХ + ФЧХ from one sweep of numina::TransferFunction::frequencyResponse(jω).
/// ω-grid: ResponseLab::frequencyRange() (or model params) + logspace/linspace.
/// If den free term is 0, ω starts strictly above 0 (no DC).
inline FrequencyBundle frequencyBundle(const numina::TransferFunction& tf, const ModelParam& p) {
    numina::ResponseLab lab(tf);
    std::pair<double, double> range =
        p.autoFreqRange ? lab.frequencyRange() : std::make_pair(p.freqMin, p.freqMax);

    // Free term D(0)=0 → W(j0) undefined / infinite; never sample ω=0.
    if (hasZeroDenConstant(tf)) {
        constexpr double w_min_floor = 1e-4;
        range.first = std::max(range.first, w_min_floor);
        if (!(range.second > range.first))
            range.second = range.first * 1e3;
    }

    const std::size_t n =
        p.autoFreqIntervals ? 120 : static_cast<std::size_t>(std::max(2, p.freqIntervals));

    const std::vector<double> omegas =
        (p.freqScale == 1 && !p.autoFreqIntervals) ? numina::core::linspace(range, n)
                                                   : numina::core::logspace(range, n, /*from_scratch=*/true);

    FrequencyBundle out;
    out.nyquist.reserve(omegas.size());
    out.amplitude.reserve(omegas.size());
    out.phase.reserve(omegas.size());

    constexpr double rad2deg = 180.0 / std::numbers::pi;
    for (const double w : omegas) {
        if (!(w > 0.0))
            continue;
        const auto W = tf.frequencyResponse({0.0, w});
        out.nyquist.push_back(W);
        out.amplitude.emplace_back(w, std::abs(W));
        out.phase.emplace_back(w, std::arg(W) * rad2deg);
    }
    return out;
}

inline bool validInput(const std::vector<double>& num, const std::vector<double>& den) {
    if (den.empty() || den.size() == 1)
        return false;
    if (num.size() > den.size())
        return false;
    return true;
}

} // namespace tf_builder

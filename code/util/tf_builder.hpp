#pragma once

#include "numina/classes/control/transfer-function.h"
#include "numina/classes/control/transfer-function/response-lab.h"
#include "numina/classes/polynomial/polynomial.h"

#include "code/model/model_param.hpp"

#include <cstdint>
#include <utility>
#include <vector>

namespace tf_builder {

inline numina::Polynomial poly(std::vector<double> coeffs) {
    if (coeffs.empty())
        coeffs = {0.0};
    return numina::Polynomial(std::move(coeffs));
}

/// num/den: high → low. Optional Padé delay.
inline numina::TransferFunction plant(std::vector<double> num, std::vector<double> den, double tau = 0.0,
                                      int order = 3) {
    if (tau == 0.0)
        return numina::TransferFunction(poly(std::move(num)), poly(std::move(den)));
    return numina::TransferFunction{
        numina::TransferFunction(poly(std::move(num)), poly(std::move(den))),
        numina::TransferFunction::makeDelay(tau, static_cast<std::uint8_t>(order)),
    };
}

/// Closed loop: cascade plant × regulator (× delay) then closeLoop().
inline numina::TransferFunction closedLoop(std::vector<double> plantNum, std::vector<double> plantDen,
                                           std::vector<double> regNum, std::vector<double> regDen, double tau = 0.0,
                                           int order = 3) {
    numina::TransferFunction open{
        numina::TransferFunction(poly(std::move(plantNum)), poly(std::move(plantDen))),
        numina::TransferFunction(poly(std::move(regNum)), poly(std::move(regDen))),
    };
    if (tau != 0.0)
        open *= numina::TransferFunction::makeDelay(tau, static_cast<std::uint8_t>(order));
    open.closeLoop();
    return open;
}

inline numina::ResponseLab::VecPair transient(const numina::TransferFunction& tf, const ModelParam& p) {
    numina::ResponseLab lab(tf);
    if (p.autoSimTime)
        return lab.transient();
    if (p.autoTimeIntervals)
        return lab.transient({0.0, static_cast<double>(p.simTime)});
    return lab.transient({0.0, static_cast<double>(p.simTime)}, static_cast<std::size_t>(p.timeIntervals));
}

inline numina::ResponseLab::VecComp frequency(const numina::TransferFunction& tf, const ModelParam& p) {
    numina::ResponseLab lab(tf);
    if (p.autoFreqRange)
        return lab.frequency();
    const auto range = std::make_pair(p.freqMin, p.freqMax);
    if (p.autoFreqIntervals)
        return lab.frequency(range, 1e-2);
    if (p.freqScale == 1)
        return lab.frequencyLinear(range, static_cast<std::size_t>(p.freqIntervals));
    return lab.frequency(range, static_cast<std::size_t>(p.freqIntervals));
}

inline bool validInput(const std::vector<double>& num, const std::vector<double>& den) {
    if (den.empty() || den.size() == 1)
        return false;
    if (num.size() > den.size())
        return false;
    return true;
}

} // namespace tf_builder

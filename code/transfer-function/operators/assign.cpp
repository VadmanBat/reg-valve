#include "../transfer-function.hpp"

TransferFunction& TransferFunction::operator=(const TransferFunction& other) {
    numerator   = other.numerator;
    denominator = other.denominator;

    roots               = other.roots;
    impulse_factors     = other.impulse_factors;
    transient_factors   = other.transient_factors;
    transientFunction   = other.transientFunction;
    impulseFunction     = other.impulseFunction;

    is_settled          = other.is_settled;
    steady_state_value  = other.steady_state_value;
    settling_time       = other.settling_time;
    rise_time           = other.rise_time;
    peak_time           = other.peak_time;
    omega_n             = other.omega_n;
    omega_c             = other.omega_c;
    zeta                = other.zeta;
    m_overshoot         = other.m_overshoot;

    return *this;
}

TransferFunction& TransferFunction::operator=(TransferFunction&& other) noexcept {
    numerator   = std::move(other.numerator);
    denominator = std::move(other.denominator);

    roots               = std::move(other.roots);
    impulse_factors     = std::move(other.impulse_factors);
    transient_factors   = std::move(other.transient_factors);
    transientFunction   = std::move(other.transientFunction);
    impulseFunction     = std::move(other.impulseFunction);

    is_settled          = other.is_settled;
    steady_state_value  = other.steady_state_value;
    settling_time       = other.settling_time;
    rise_time           = other.rise_time;
    peak_time           = other.peak_time;
    omega_n             = other.omega_n;
    omega_c             = other.omega_c;
    zeta                = other.zeta;
    m_overshoot         = other.m_overshoot;

    return *this;
}

void TransferFunction::assign(const Vec& newNumerator, const Vec& newDenominator) {
    numerator   = newNumerator;
    denominator = newDenominator;

    roots = numina::solve_polynomial_laguerre(denominator);
    recomputeBackState();
}

void TransferFunction::assign(Vec&& newNumerator, Vec&& newDenominator) {
    numerator   = std::move(newNumerator);
    denominator = std::move(newDenominator);

    roots = numina::solve_polynomial_laguerre(denominator);
    recomputeBackState();
}

void TransferFunction::assign(const TransferFunction& other) {
    numerator   = other.numerator;
    denominator = other.denominator;

    roots               = other.roots;
    impulse_factors     = other.impulse_factors;
    transient_factors   = other.transient_factors;
    transientFunction   = other.transientFunction;
    impulseFunction     = other.impulseFunction;

    is_settled          = other.is_settled;
    steady_state_value  = other.steady_state_value;
    settling_time       = other.settling_time;
    rise_time           = other.rise_time;
    peak_time           = other.peak_time;
    omega_n             = other.omega_n;
    omega_c             = other.omega_c;
    zeta                = other.zeta;
    m_overshoot         = other.m_overshoot;
}

void TransferFunction::assign(TransferFunction&& other) {
    numerator   = std::move(other.numerator);
    denominator = std::move(other.denominator);

    roots               = std::move(other.roots);
    impulse_factors     = std::move(other.impulse_factors);
    transient_factors   = std::move(other.transient_factors);
    transientFunction   = std::move(other.transientFunction);
    impulseFunction     = std::move(other.impulseFunction);

    is_settled          = other.is_settled;
    steady_state_value  = other.steady_state_value;
    settling_time       = other.settling_time;
    rise_time           = other.rise_time;
    peak_time           = other.peak_time;
    omega_n             = other.omega_n;
    omega_c             = other.omega_c;
    zeta                = other.zeta;
    m_overshoot         = other.m_overshoot;
}
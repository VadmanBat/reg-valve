#include "../transfer-function.hpp"

TransferFunction& TransferFunction::operator=(TransferFunction other) {
    numerator   = std::move(other.numerator);
    denominator = std::move(other.denominator);

    roots               = std::move(other.roots);
    impulse_factors     = std::move(other.impulse_factors);
    transient_factors   = std::move(other.transient_factors);

    is_settled          = other.is_settled;
    settling_time       = other.settling_time;
    steady_state_value  = other.steady_state_value;

    return *this;
}

TransferFunction& TransferFunction::operator=(TransferFunction&& other) noexcept {
    numerator   = std::move(other.numerator);
    denominator = std::move(other.denominator);

    roots               = std::move(other.roots);
    impulse_factors     = std::move(other.impulse_factors);
    transient_factors   = std::move(other.transient_factors);

    is_settled          = other.is_settled;
    settling_time       = other.settling_time;
    steady_state_value  = other.steady_state_value;

    return *this;
}

void TransferFunction::assign(Vec newNumerator, Vec newDenominator) {
    numerator   = std::move(newNumerator);
    denominator = std::move(newDenominator);

    roots = SupMathCore::solvePolynomialLaguerre(denominator);
    recomputeBackState();
}

void TransferFunction::assign(Vec&& newNumerator, Vec&& newDenominator) {
    numerator   = std::move(newNumerator);
    denominator = std::move(newDenominator);

    roots = SupMathCore::solvePolynomialLaguerre(denominator);
    recomputeBackState();
}

void TransferFunction::assign(TransferFunction other) {
    numerator   = std::move(other.numerator);
    denominator = std::move(other.denominator);

    roots               = std::move(other.roots);
    impulse_factors     = std::move(other.impulse_factors);
    transient_factors   = std::move(other.transient_factors);

    is_settled          = other.is_settled;
    settling_time       = other.settling_time;
    steady_state_value  = other.steady_state_value;
}

void TransferFunction::assign(TransferFunction&& other) {
    numerator   = std::move(other.numerator);
    denominator = std::move(other.denominator);

    roots               = std::move(other.roots);
    impulse_factors     = std::move(other.impulse_factors);
    transient_factors   = std::move(other.transient_factors);

    is_settled          = other.is_settled;
    settling_time       = other.settling_time;
    steady_state_value  = other.steady_state_value;
}
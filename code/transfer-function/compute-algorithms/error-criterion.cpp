#include "../transfer-function.hpp"

TransferFunction::Type TransferFunction::computeLinearIntegralCriterion(const VecPair& response) const {
    Type sum = 0;
    const auto n = response.size();
    for (std::size_t i = 1; i < n; ++i)
        sum += (response[i].first - response[i - 1].first) *
                std::abs((response[i - 1].second + response[i].second) / 2 - steady_state_value);
    return sum;
} /// N

TransferFunction::Type TransferFunction::computeIntegralQuadraticCriterion() const {
    const auto [coefficients, exponents] =
            MathCore::squareExponentialSum(transient_factors, roots);
    const auto n = coefficients.size();
    Complex sum = 0;
    for (std::size_t i = 0; i < n; ++i)
        sum += coefficients[i] / exponents[i] * (std::exp(exponents[i] * settling_time) - Complex(1));
    return sum.real();
} /// size(roots)^2

TransferFunction::Type TransferFunction::computeStandardDeviation(const std::size_t points) const {
    Type sum = 0, error;
    const Type step = settling_time / static_cast<Type>(points - 1);
    for (std::size_t i = 0; i < points; ++i) {
        error = transientFunction(static_cast<Type>(i) * step) - steady_state_value;
        sum += error * error;
    }
    return std::sqrt(sum / static_cast<Type>(points - 1));
} /// N
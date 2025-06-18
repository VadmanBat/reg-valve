//
// Created by Vadim on 18.07.2024.
//

#ifndef REGVALVE_TRANSFER_FUNCTION_HPP
#define REGVALVE_TRANSFER_FUNCTION_HPP

#include "../math-core.hpp"
#include "../reg-core.hpp"

#include "code/numina/polynomial/solve.hpp"
#include "code/numina/fraction/invlaplace.hpp"
#include "code/numina/terms/term-expression.hpp"

#include <map>

/// Класс передаточной функции W(p)
class TransferFunction {
private:
    using Type      = ProjectTypes::Type;
    using Complex   = ProjectTypes::Complex;

    using Vec       = std::vector <Type>;
    using Pair      = std::pair <Type, Type>;
    using VecPair   = std::vector <Pair>;
    using VecComp   = std::vector <Complex>;

    class Roots {
        struct comp {
            bool operator()(const auto& a, const auto& b) const {
                return a.real() != b.real() ? a.real() > b.real() : a.imag() < b.imag();
            }
        };

        std::map <Complex, int, comp> getRoots(VecComp roots) {
            static const Type epsilon = std::sqrt(std::numeric_limits<Type>::epsilon());
            const auto n = roots.size();
            std::sort(roots.begin(), roots.end(), comp());
            int powers = 1;
            std::map <Complex, int, comp> res;
            for (std::size_t i = 1; i < n; ++i) {
                if (std::abs(roots[i - 1] - roots[i]) < epsilon) {
                    ++powers;
                    continue;
                }
                res[roots[i - 1]] = powers;
                powers = 1;
            }
            res[roots[n - 1]] = powers;
            return res;
        }
    };

    Vec numerator, denominator;
    VecComp roots, impulse_factors, transient_factors;

    bool is_settled{};
    Type steady_state_value{};
    Type settling_time{}, rise_time{}, peak_time{};
    Type omega_n{}, omega_c{}, zeta{}, m_overshoot{};

    void recomputeFrontState() {
        roots.emplace_back(0);
        transient_factors = numina::simpleParfrac(numerator, roots, denominator.front());
        transientFunction = TermExpression(roots, transient_factors, std::vector <int>(roots.size(), 0));
        //transientFunction = numina::invlaplace(numerator, denominator, roots);
        impulseFunction = transientFunction.derivative();
        steady_state_value = transient_factors.back().real();
        transient_factors.pop_back();
        roots.pop_back();

        impulse_factors = transient_factors;
        const auto n = roots.size();
        for (std::size_t i = 0; i < n; ++i)
            impulse_factors[i] *= roots[i];

        m_overshoot = std::abs(transientFunction(peak_time) - steady_state_value) / steady_state_value * 100;
    }

    void recomputeBackState() {
        std::sort(roots.begin(), roots.end(),
        [](const auto& a, const auto& b){
            return a.real() != b.real() ? a.real() > b.real() : a.imag() < b.imag();
        });
        const Complex& dominant_root(roots[0]);
        if (dominant_root.real() < 0) {
            is_settled      = true;
            settling_time   = -4 / dominant_root.real();
            omega_n         = std::abs(dominant_root);
            zeta            = -dominant_root.real() / omega_n;
            Type zeta_sq    = zeta * zeta;
            rise_time       = 1.8 / omega_n;
            peak_time       = std::numbers::pi / (omega_n * std::sqrt(1 - zeta_sq));
            omega_c         = omega_n * std::sqrt(1 - 2 * zeta_sq + std::sqrt(4 * zeta_sq * (zeta_sq - 1) + 2));
        }
        else {
            is_settled  = false;
            peak_time   = 0;
        }

        recomputeFrontState();
    }

public:
    TermExpression <Type> transientFunction, impulseFunction;
    TransferFunction() = default;
    TransferFunction(const Vec& numerator, const Vec& denominator);
    [[maybe_unused]] TransferFunction(Vec&& numerator, Vec&& denominator);
    [[maybe_unused]] TransferFunction(const Vec& num, const Vec& den, double tau, int order);
    [[maybe_unused]] TransferFunction(Vec&& num, Vec&& den, double tau, int order);
    [[maybe_unused]] TransferFunction(const Vec& objNum, const Vec& objDen, const Vec& regNum, const Vec& regDen);
    [[maybe_unused]] TransferFunction(const Vec& objNum, const Vec& objDen, const Vec& regNum, const Vec& regDen, double tau, int order);

    [[nodiscard]] inline bool isSettled() const {
        return is_settled;
    }
    [[nodiscard]] inline Type steadyStateValue() const {
        return steady_state_value;
    }
    [[nodiscard]] inline Type settlingTime() const {
        return settling_time;
    }
    [[nodiscard]] inline Type riseTime() const {
        return rise_time;
    }
    [[nodiscard]] inline Type peakTime() const {
        return peak_time;
    }
    [[nodiscard]] inline Type naturalFrequency() const {
        return omega_n;
    }
    [[nodiscard]] inline Type cutFrequency() const {
        return omega_c;
    }
    [[nodiscard]] inline Type dampingRation() const {
        return zeta;
    }
    [[nodiscard]] inline Type overshoot() const {
        return m_overshoot;
    }
    [[nodiscard]] inline VecComp getRoots() const {
        return roots;
    }

    [[nodiscard]] inline Vec getNumerator() const {
        return numerator;
    }

    [[nodiscard]] inline Vec getDenominator() const {
        return denominator;
    }

    void setNumerator(Vec newNumerator) {
        numerator = std::move(newNumerator);
        recomputeFrontState();
    }

    void setDenominator(Vec newDenominator) {
        denominator = std::move(newDenominator);
        roots = numina::solve_polynomial_laguerre(denominator);
        recomputeBackState();
    }

    /*[[nodiscard]] inline Type transientResponse(const Type& time) const {
        Complex result = steady_state_value;
        const std::size_t n = transient_factors.size();
        for (std::size_t i = 0; i < n; ++i)
            result += transient_factors[i] * std::exp(roots[i] * time);
        return result.real();
    }

    [[nodiscard]] inline Type impulseResponse(const Type& time) const {
        Complex result = 0;
        const std::size_t n = impulse_factors.size();
        for (std::size_t i = 0; i < n; ++i)
            result += impulse_factors[i] * std::exp(roots[i] * time);
        return result.real();
    }*/

    [[nodiscard]] inline Complex frequencyResponse(const Complex& p) const {
        Complex num(0);
        Complex den(0);

        for (const auto& x : numerator) {
            num *= p;
            num += x;
        }
        for (const auto& x : denominator) {
            den *= p;
            den += x;
        }

        return num / den;
    }

    [[nodiscard]] VecPair transientResponse() const;
    [[nodiscard]] VecPair transientResponse(const Pair& range, Type tolerance = 1e-2) const;
    [[nodiscard]] VecPair transientResponse(const Pair& range, std::size_t points) const;

    [[nodiscard]] VecPair impulseResponse() const;
    [[nodiscard]] VecPair impulseResponse(const Pair& range, Type tolerance = 1e-2) const;
    [[nodiscard]] VecPair impulseResponse(const Pair& range, std::size_t points) const;

    [[nodiscard]] VecComp frequencyResponse() const;
    [[nodiscard]] VecComp frequencyResponse(const Pair& range, Type tolerance = 1e-2) const;
    [[nodiscard]] VecComp frequencyResponse(const Pair& range, std::size_t points) const;
    [[nodiscard]] VecComp frequencyResponse(const Pair& range, std::size_t points, bool) const;

    [[nodiscard]] VecComp amplitudeFrequencyResponse() const;
    [[nodiscard]] VecComp phaseFrequencyResponse() const;

    [[nodiscard]] Type binarySearchFrequency(Type min, Type max, Type value, Type tolerance = 1e-3) const; /// log(N)
    [[nodiscard]] Type ternarySearchFrequency(Type min, Type max, Type tolerance = 1e-3) const; /// log(N)
    [[nodiscard]] Pair computeFrequencyRange(Type min = 0, Type max = 1e2) const; /// log(N)

    [[nodiscard]] Type computeLinearIntegralCriterion(const VecPair& response) const; /// N
    [[nodiscard]] Type computeIntegralQuadraticCriterion() const; /// size(roots)^2
    [[nodiscard]] Type computeStandardDeviation(std::size_t points = 100) const; /// N

    void closeLoop() {
        const std::size_t n = numerator.size();
        const std::size_t m = denominator.size();
        if (n >= m) {
            denominator.resize(n, 0);
            for (std::size_t i = 0; i < n; ++i)
                denominator[i] += numerator[i];
        }
        else {
            const std::size_t delta = denominator.size() - n;
            for (std::size_t i = 0; i < n; ++i)
                denominator[delta + i] += numerator[i];
        }
        roots = numina::solve_polynomial_laguerre(denominator);
        recomputeBackState();
    }

    TransferFunction& operator=(const TransferFunction& other);
    TransferFunction& operator=(TransferFunction&& other) noexcept;
    void assign(const Vec& newNumerator, const Vec& newDenominator);
    void assign(Vec&& newNumerator, Vec&& newDenominator);
    void assign(const TransferFunction& other);
    void assign(TransferFunction&& other);

    TransferFunction& operator*=(const TransferFunction& other) {
        numerator   = MathCore::multiply(numerator, other.numerator);
        denominator = MathCore::multiply(denominator, other.denominator);

        roots = numina::solve_polynomial_laguerre(denominator);
        recomputeBackState();

        return *this;
    }
};

#endif //REGVALVE_TRANSFER_FUNCTION_HPP

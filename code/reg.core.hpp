//
// Created by Vadma on 17.07.2024.
//

#ifndef REGVALVE_REG_CORE_HPP
#define REGVALVE_REG_CORE_HPP

#include <utility>

#include "math-core.hpp"

class RegCore {
public:
    using Type      = ProjectTypes::Type;
    using Complex   = ProjectTypes::Complex;

    using Vec       = std::vector <Type>;
    using Pair      = std::pair <Type, Type>;
    using VecPair   = std::vector <Pair>;
    using VecComp   = std::vector <Complex>;
    using ValComp   = std::valarray <Complex>;

    template <Type tolerance = 1e-3>
    static Type binarySearchFrequency(Type min, Type max, const Type& value,
                                      const std::function<Complex(const Complex& p)>& function,
                                      const Type& epsilon = 1e-2) {
        Type mid((min + max) / 2);
        while (max - min > tolerance) {
            Type response(std::abs(function(mid)));

            if (std::abs(response - value) / value < epsilon)
                return mid;

            (response > value ? min : max) = mid;
            mid = (min + max) / 2;
        }

        return mid;
    }

    static Pair computeFrequencyRange(const Vec& numeratorCoeffs, /// Коэффициенты числителя (от большей степени к меньшей)
                                      const Vec& denominatorCoeffs, /// Коэффициенты знаменателя (от большей степени к меньшей))
                                      const Type& epsilon = 1e-3)
    {
        auto W = [&numeratorCoeffs, &denominatorCoeffs](const Complex& p) {
            Complex numerator(0);
            Complex denominator(0);

            for (const auto& x : numeratorCoeffs) {
                numerator *= p;
                numerator += x;
            }
            for (const auto& x : denominatorCoeffs) {
                denominator *= p;
                denominator += x;
            }

            return numerator / denominator;
        };

        const Type initValue(abs(W(0)));
        return {
                binarySearchFrequency(0, 100, initValue, W, epsilon),
                binarySearchFrequency(0, 100, initValue * epsilon, W, epsilon)
        };
    }

    static VecPair calculateTransietResponse(const std::vector <Type>& numerator,
                                             const std::vector <Type>& denominator,
                                             const std::vector <Type>& time)
    {
        std::vector <Complex> num(ConvertCore::toComplexVector(numerator));
        auto den(denominator);

        //den.push_back(0);
        auto roots(MathCore::solvePolynomialNewton(den));
        roots.emplace_back(0);

        for (auto root : roots)
            std::cout << "root = " << root << '\n';

        auto coefficients(MathCore::computeCoefficients(num, roots));
        const std::size_t degree(roots.size());

        for (auto coeff : coefficients)
            std::cout << coeff << '\n';

        auto h = [degree, &coefficients, &roots](Complex t){
            Complex result(0);
            for (std::size_t i = 0; i < degree; ++i)
                result += coefficients[i] * std::exp(roots[i] * t);
            return result;
        }; /// N

        VecPair res;
        res.reserve(time.size());
        for (const auto& t : time)
            res.emplace_back(t, h(t).real());

        return res;
    } /// N * log(N) + N^2 * N * log(N) + N^3 + M

    /// Функция для вычисления комплексно частотной характеристики (КЧХ)
    /// по передаточной функции W(p) объекта управления (ОУ)
    static VecComp calculateFrequencyResponse(
            const Vec& numeratorCoeffs,        /// Коэффициенты числителя (от большей степени к меньшей)
            const Vec& denominatorCoeffs,      /// Коэффициенты знаменателя (от большей степени к меньшей)
            const Vec& frequency               /// Угловая частота (рад/сек)
    )
    {
        VecComp frequencyResponse;
        frequencyResponse.reserve(frequency.size());

        auto W = [&numeratorCoeffs, &denominatorCoeffs](const Complex& p) {
            Complex numerator(0);
            Complex denominator(0);

            for (const auto& x : numeratorCoeffs) {
                numerator *= p;
                numerator += x;
            }
            for (const auto& x : denominatorCoeffs) {
                denominator *= p;
                denominator += x;
            }

            return numerator / denominator;
        };

        for (const auto& omega : frequency)
            frequencyResponse.push_back(W(Complex(0, omega))); /// p = jw
        return frequencyResponse;
    } /// count(frequency) * (N + M)
};

/// Класс передаточной функции
class TransferFunction {
private:
    using Type      = ProjectTypes::Type;
    using Complex   = ProjectTypes::Complex;

    using Vec       = std::vector <Type>;
    using Pair      = std::pair <Type, Type>;
    using VecPair   = std::vector <Pair>;
    using VecComp   = std::vector <Complex>;
    using ValComp   = std::valarray <Complex>;

    Vec numerator, denominator;
    VecComp roots, impulse_factors, transient_factors;

    bool is_settled;
    Type settling_time, steady_state_value;

    void recomputeState() {
        std::sort(roots.begin(), roots.end(), [](const auto& a, const auto& b){
            if (a.real() != b.real())
                return a.real() < b.real();
            return a.imag() < b.imag();
        });

        if (roots.back().real() < 0) {
            is_settled = true;
            settling_time = 4 / roots.front().real();
            steady_state_value = -1;
        }
        else {
            steady_state_value = false;
            settling_time = -1;
            steady_state_value = 0;
        }

        impulse_factors = MathCore::computeCoefficients(numerator, roots);
        roots.emplace_back(0);
        transient_factors = MathCore::computeCoefficients(numerator, roots);
        roots.pop_back();
        steady_state_value = transient_factors.back().real();
        transient_factors.pop_back();
    }

public:
    TransferFunction(Vec numerator, Vec denominator) :
            numerator(std::move(numerator)), denominator(std::move(denominator)),
            roots(MathCore::solvePolynomialNewton(denominator))
    {
        recomputeState();
    }

    TransferFunction(Vec&& numerator, Vec&& denominator) :
            numerator(std::move(numerator)), denominator(std::move(denominator)),
            roots(MathCore::solvePolynomialNewton(denominator))
    {
        recomputeState();
    }

    inline bool isSettled() const {
        return is_settled;
    }

    inline Type settlingTime() const {
        return settling_time;
    }

    inline Type steadyStateValue() const {
        return steady_state_value;
    }

    Complex operator()(const Complex& p) const {
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

    Type transientResponse(const Complex& p) const {
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

        return (num / den).real();
    }

    Type impulseResponse(const Type& time) const {
        Complex result(0);
        const std::size_t n(impulse_factors.size());
        for (std::size_t i = 0; i < n; ++i)
            result += impulse_factors[i] * std::exp(roots[i] * time);
        return result.real();
    }

    Complex frequencyResponse(const Type& time) const {
        Complex result(steady_state_value);
        const std::size_t n(transient_factors.size());
        for (std::size_t i = 0; i < n; ++i)
            result += transient_factors[i] * std::exp(roots[i] * time);
        return result.real();
    }

    template <Type tolerance = 1e-3>
    Type binarySearchFrequency(Type min, Type max, const Type& value, const Type& epsilon = 1e-2) const {
        Type mid((min + max) / 2);
        while (max - min > tolerance) {
            Type response(std::abs(frequencyResponse(mid)));

            if (std::abs(response - value) / value < epsilon)
                return mid;

            (response > value ? min : max) = mid;
            mid = (min + max) / 2;
        }

        return mid;
    }

    Pair computeFrequencyRange(const Type& min = 0, const Type& max = 1e2, const Type& epsilon = 1e-3) const {
        const Type initValue(abs(frequencyResponse(0)));
        return {
                binarySearchFrequency(min, max, initValue, epsilon),
                binarySearchFrequency(min, max, initValue * epsilon, epsilon)
        };
    }

    Type computeLinearIntegralCriterion(const std::size_t points = 100) const {
        Type sum(0);
        const Type step(settling_time / static_cast<Type>(points - 1));
        for (std::size_t i = 0; i < points; ++i)
            sum += std::abs(transientResponse(i * step) - steady_state_value);
        return sum * step;
    }

    Type computeIntegralQuadraticCriterion(const std::size_t points = 100) const {
        Type sum(0), error;
        const Type step(settling_time / static_cast<Type>(points - 1));
        for (std::size_t i = 0; i < points; ++i) {
            error = transientResponse(i * step) - steady_state_value;
            sum += error * error;
        }
        return sum * step;
    }

    Type computeStandardDeviation(const std::size_t points = 100) const {
        Type sum(0), error;
        const Type step(settling_time / static_cast<Type>(points - 1));
        for (std::size_t i = 0; i < points; ++i) {
            error = transientResponse(i * step) - steady_state_value;
            sum += error * error;
        }
        return std::sqrt(sum / static_cast<Type>(points - 1));
    }
};

#endif //REGVALVE_REG_CORE_HPP

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

        auto coefficients(MathCore::computeFactorsSimpleFractions(num, roots, denominator.front()));
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

#endif //REGVALVE_REG_CORE_HPP
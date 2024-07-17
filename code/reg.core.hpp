//
// Created by Vadma on 17.07.2024.
//

#ifndef REGVALVE_REG_CORE_HPP
#define REGVALVE_REG_CORE_HPP

#include "math-core.hpp"

class RegCore {
public:
    using Type      = ProjectTypes::Type;
    using Complex   = ProjectTypes::Complex;

    using Vec       = std::vector <Type>;
    using VecPair   = std::vector <std::pair <Type, Type>>;
    using VecComp   = std::vector <Complex>;
    using ValComp   = std::valarray <Complex>;

    static std::pair <Type, Type> computeFrequencyRange(const Vec& numeratorCoeffs, /// Коэффициенты числителя (от большей степени к меньшей)
                                                        const Vec& denominatorCoeffs, /// Коэффициенты знаменателя (от большей степени к меньшей))
                                                        const Type& epsilon = 1e-3)
    {
        Type min, max;

        auto W = [&numeratorCoeffs, &denominatorCoeffs](const Type& w) {
            Complex s(0, w); /// s = jw
            Complex numerator(0);
            Complex denominator(0);

            for (const auto& x : numeratorCoeffs) {
                numerator *= s;
                numerator += x;
            }
            for (const auto& x : denominatorCoeffs) {
                denominator *= s;
                denominator += x;
            }

            return numerator / denominator;
        };

        Type start(epsilon * numeratorCoeffs.back());
        Type a(0.01), b(200), middle((a + b) / 2);
        while (std::abs(std::abs(W(middle)) - start) / start > epsilon) {
            middle = (a + b) / 2;
            (std::abs(W(middle)) > start ? a : b) = middle;
        }
        max = middle;

        return {min, max};
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

        auto W = [&numeratorCoeffs, &denominatorCoeffs](const Type& w) {
            Complex s(0, w); /// s = jw
            Complex numerator(0);
            Complex denominator(0);

            for (const auto& x : numeratorCoeffs) {
                numerator *= s;
                numerator += x;
            }
            for (const auto& x : denominatorCoeffs) {
                denominator *= s;
                denominator += x;
            }

            return numerator / denominator;
        };

        for (const auto& omega : frequency)
            frequencyResponse.push_back(W(omega));
        return frequencyResponse;
    } /// count(w) * (N + M)
};

#endif //REGVALVE_REG_CORE_HPP

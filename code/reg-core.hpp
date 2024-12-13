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

        auto coefficients(MathCore::decomposeFraction(num, roots, denominator.front()));
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

    static VecComp fft(const VecComp& x) {
        const auto n = x.size();
        if (n <= 1) return x;

        VecComp even(n / 2);
        VecComp odd(n / 2);

        for (std::size_t i = 0; i < n; i += 2) {
            even[i / 2] = x[i];
            odd[i / 2] = x[i + 1];
        }

        VecComp even_fft = fft(even);
        VecComp odd_fft = fft(odd);

        VecComp y(n);
        for (std::size_t k = 0; k < n / 2; ++k) {
            Complex wk = std::polar(1.0, -2.0 * M_PI * k / n);
            y[k]            = even_fft[k] + wk * odd_fft[k];
            y[k + n / 2]    = even_fft[k] - wk * odd_fft[k];
        }

        return y;
    }

    static VecComp ifft(const VecComp& x) {
        const auto n = x.size();
        VecComp y = x;
        for (std::size_t i = 0; i < n; ++i)
            y[i] = conj(y[i]);
        y = fft(y);
        for (int i = 0; i < n; ++i)
            y[i] = conj(y[i]) / double(n);
        return y;
    }

    static VecComp calculateFrequencyResponse(const VecPair& h, double samplingRate) {
        if (h.empty())
            throw std::runtime_error("Input vector h_t is empty.");

        const auto n = h.size();

        VecComp fft_result(n);
        for (size_t i = 0; i < n; ++i)
            fft_result[i] = h[i].second;
        fft_result = fft(fft_result);

        VecComp frequencies(n / 2 + 1);
        VecComp phase(n / 2 + 1);
        VecComp amplitude(n / 2 + 1);
        VecComp complex(n / 2 + 1);

        for (size_t k = 0; k <= n / 2; ++k) {
            frequencies[k] = double(k) * samplingRate / double(n);
            phase[k] = std::atan2(fft_result[k].imag(), fft_result[k].real());
            phase[k] = std::arg(fft_result[k]);
            amplitude[k] = std::abs(fft_result[k]);
            std::cout << amplitude[k] << '\n';
            complex[k] = phase[k] / amplitude[k];
        }

        return complex;
    }
};

#endif //REGVALVE_REG_CORE_HPP
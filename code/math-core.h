//
// Created by Vadma on 08.07.2024.
//

#ifndef REGVALVE_MATH_CORE_H
#define REGVALVE_MATH_CORE_H

#include <vector>
#include <complex>
#include <valarray>

#include <iostream>

class MathCore {
public:
    using Type      = double;
    using Complex   = std::complex <Type>;

    using Vec       = std::vector <Type>;
    using VecPair   = std::vector <std::pair <Type, Type>>;
    using VecComp   = std::vector <Complex>;
    using ValComp   = std::valarray <Complex>;

    static Vec range(
        const Type& first,
        const Type& last,
        const Type& step
    )
    {
        const int points((last - first) / step);
        Vec res(points);
        for (int i = 0; i < points; ++i)
            res[i] = first + i * step;
        return res;
    }

    /// Функция для вычисления комплексно частотной характеристики (КЧХ)
    /// по передаточной функции W(p) объекта управления (ОУ)
    static VecComp calculateFrequencyResponse(
        const Vec& numeratorCoeffs,        /// Коэффициенты числителя (обратные)
        const Vec& denominatorCoeffs,      /// Коэффициенты знаменателя (обратные)
        const Vec& frequency               /// Угловая частота (рад/сек)
    )
    {
        VecComp frequencyResponse;
        frequencyResponse.reserve(frequency.size());

        for (const auto& w : frequency) {
            Complex s(0, w); /// p = jw
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

            frequencyResponse.push_back(numerator / denominator);
        }
        return frequencyResponse;
    }

    template <typename Type>
    static std::vector <Type> multiply(const std::vector <Type>& coefficients) {
        const std::size_t n(coefficients.size());
        std::vector <Type> res(n + 1);
        res[0] = 1;
        res[1] = coefficients[0];
        for (std::size_t i = 1; i < n; ++i)
            for (std::size_t j = i + 1; j >= 1; --j)
                res[j] += coefficients[i] * res[j - 1];
        return res;
    }

    template <typename Type>
    static std::vector <Type> solveMatrix(Type* matrix, std::size_t n) {
        for (std::size_t i = 0; i < n; ++i, std::cout << '\n')
            for (std::size_t j = 0; j <= n; ++j)
                std::cout << matrix[i * (n + 1) + j] << ' ';

        for (std::size_t i = 0; i < n; ++i) {
            Type diagElement(matrix[i * (n + 1) + i]);
            for (std::size_t j = 0; j <= n; ++j)
                matrix[i * (n + 1) + j] /= diagElement;

            for (std::size_t k = 0; k < i; ++k) {
                Type factor(matrix[k * (n + 1) + i]);
                for (std::size_t j = 0; j <= n; ++j)
                    matrix[k * (n + 1) + j] -= factor * matrix[i * (n + 1) + j];
            }
            for (std::size_t k = i + 1; k < n; ++k) {
                Type factor(matrix[k * (n + 1) + i]);
                for (std::size_t j = 0; j <= n; ++j)
                    matrix[k * (n + 1) + j] -= factor * matrix[i * (n + 1) + j];
            }
        }
        std::vector <Type> answers(n);
        for (std::size_t i = 0; i < n; ++i)
            answers[i] = matrix[i * (n + 1) + n];
        return answers;
    }

    static VecComp computeCoefficients(const VecComp& numerator, const VecComp& roots) {
        std::size_t n(roots.size());
        auto num(numerator);
        num.resize(n, 0);
        Complex matrix[n * n + n];
        for (std::size_t i = 0; i < n; ++i) {
            VecComp x;
            x.reserve(n - 1);
            for (std::size_t j = 0; j < i; ++j)
                x.push_back(-roots[j]);
            for (std::size_t j = i + 1; j < n; ++j)
                x.push_back(-roots[j]);
            auto coeffs(multiply(x));
            for (std::size_t j = 0; j < n; ++j)
                matrix[j * (n + 1) + i] = coeffs[j];
        }
        for (std::size_t i = 0; i < n; ++i)
            matrix[i * (n + 1) + n] = num[n - 1 - i];
        return solveMatrix(matrix, n);
    }

    template <typename Type>
    static std::vector <Complex> toComplexVector(const std::vector <Type>& numbers) {
        std::vector <Complex> res;
        res.reserve(numbers.size());
        for (const auto& number : numbers)
            res.push_back(number);
        return res;
    }

    static VecPair calculateTransietResponse(const std::vector <Type>& numerator,
                                             const std::vector <Type>& denominator,
                                             const std::vector <Type>& time)
    {
        std::vector <Complex> num(toComplexVector(numerator));
        auto den(denominator);

        //den.push_back(0);
        auto roots(solvePolynomialNewton(den));
        roots.emplace_back(0);

        for (auto root : roots)
            std::cout << "root = " << root << '\n';

        auto coefficients(computeCoefficients(num, roots));
        const std::size_t degree(roots.size());

        for (auto coeff : coefficients)
            std::cout << coeff << '\n';

        auto h = [degree, &coefficients, &roots](Complex t){
            Complex result(0);
            for (std::size_t i = 0; i < degree; ++i)
                result += coefficients[i] * std::exp(roots[i] * t);
            return result;
        };

        VecPair res;
        res.reserve(time.size());
        for (const auto& t : time)
            res.emplace_back(t, h(t).real());

        return res;
    }

    template <typename Type>
    static std::vector <Type> deflatePolynomial(const std::vector <Type>& coefficients, const Type& root) {
        const std::size_t n(coefficients.size() - 1);
        std::vector <Type> newCoefficients(n);
        newCoefficients[0] = coefficients[0];

        for (std::size_t i = 1; i < n; ++i)
            newCoefficients[i] = coefficients[i] + root * newCoefficients[i - 1];

        return newCoefficients;
    }

    static Complex normalize(const Complex& z, const Type& epsilon = 1e-9) {
        Type real = (std::abs(z.real()) < epsilon) ? 0 : z.real();
        Type imag = (std::abs(z.imag()) < epsilon) ? 0 : z.imag();
        return {real, imag};
    }

    template <int EPSILON_ORDER = 9, std::size_t MAX_ITER = 100>
    static VecComp solvePolynomialNewton(const Vec& coefficients) {
        static const long double epsilon(std::pow(10.L, -EPSILON_ORDER));

        auto coeffs(toComplexVector(coefficients));
        std::size_t degree(coeffs.size() - 1);
        auto f = [&coeffs, &degree](const Complex& x) {
            Complex result(0);
            for (std::size_t i = 0; i <= degree; ++i) {
                result *= x;
                result += coeffs[i];
            }
            return result;
        };
        auto df = [&coeffs, &degree](const Complex& x) {
            Complex result(0);
            for (std::size_t i = 0; i < degree; ++i) {
                result *= x;
                result += Complex(degree - i) * coeffs[i];
            }
            return result;
        };

        VecComp roots;
        while (degree > 1) {
            Complex x(1, 1);
            for (std::size_t i = 0; i < MAX_ITER; ++i) {
                Complex fx(f(x));
                Complex dfx(df(x));
                if (std::abs(fx) < epsilon)
                    break;
                x -= fx / dfx;
            }

            roots.push_back(x);
            coeffs = deflatePolynomial(coeffs, x);
            --degree;
        }
        roots.push_back(-coeffs[1] / coeffs[0]);

        for (auto& root : roots)
            root = normalize(root, epsilon);

        return roots;
    }

    template <typename Type>
    static Type polynomial(const std::vector <Type>& coefficients, const Type& x) {
        const std::size_t degree(coefficients.size() - 1);
        Type result(0);
        for (int i = 0; i <= degree; ++i) {
            result *= x;
            result += coefficients[i];
        }
        return result;
    }
};

#endif //REGVALVE_MATH_CORE_H

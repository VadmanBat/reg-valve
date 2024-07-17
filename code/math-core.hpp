//
// Created by Vadma on 08.07.2024.
//

#ifndef REGVALVE_MATH_CORE_HPP
#define REGVALVE_MATH_CORE_HPP

#include "convert-core.hpp"

#include <valarray>

#include <iostream>

/// Базовый класс с математическими операциями, включает в себя следующее:
///     - формирование диапазонов (постоянный, логарифмический шаг);
///     - вычисление значения полинома в определённом точке;
///     - перемножение полиномов первого порядка;
///     - дефляция полинома при известном корне;
///     - вычисление корней полинома методом Ньютона;
///     - решение матрицы (методом Гаусса-Жордана);
///     - вычисление коэффициентов простых слагаемых для разложения дробию.

class MathCore {
public:
    using Type      = ProjectTypes::Type;
    using Complex   = ProjectTypes::Complex;

    using Vec       = std::vector <Type>;
    using VecPair   = std::vector <std::pair <Type, Type>>;
    using VecComp   = std::vector <Complex>;
    using ValComp   = std::valarray <Complex>;

    template <class ContainerType = Vec>
    static ContainerType range(/// Формирование диапазона с постоянным шагом разбиения
        const Type& min,            /// Минимум диапазона
        const Type& max,            /// Максимум диапазона
        const std::size_t& points)  /// Количество точек разбиения
    {
        const Type step((max - min) / static_cast<Type>(points - 1));
        ContainerType res;
        res.reserve(points);
        for (int i = 0; i < points; ++i)
            res.emplace_back(min + i * step);
        return res;
    } /// N

    template <class ContainerType = Vec>
    static Vec logRange(        /// Формирование диапазона с логарифмическим шагом разбиения
            const Type& min,            /// Минимум диапазона
            const Type& max,            /// Максимум диапазона
            const std::size_t& points,  /// Количество точек разбиения
            bool isFromScratch = false)
    {
        ContainerType res;
        res.reserve(points + isFromScratch);

        if (isFromScratch)
            res.emplace_back(0);

        const Type multiplier(std::pow(10,
                                       (std::log10(max) - std::log10(min))
                                       / static_cast<Type>(points - 1)));

        Type current(min);
        for (std::size_t i = 0; i < points; ++i) {
            res.emplace_back(current);
            current *= multiplier;
        }

        return res;
    } /// N

    template <typename Type>
    static Type polynomial(const std::vector <Type>& coefficients, const Type& x) {
        Type result(0);
        const std::size_t n(coefficients.size());
        for (std::size_t i = 0; i < n; ++i) {
            result *= x;
            result += coefficients[i];
        }
        return result;
    } /// N

    /// Вычисление полинома при перемножении простых скобок (x - r1)(x - r2)
    template <typename Type>
    static std::vector <Type> multiplySimpleFractions(const std::vector <Type>& coefficients) {
        const std::size_t n(coefficients.size());
        std::vector <Type> res(n + 1);
        res[0] = 1;
        res[1] = coefficients[0];
        for (std::size_t i = 1; i < n; ++i)
            for (std::size_t j = i + 1; j >= 1; --j)
                res[j] += coefficients[i] * res[j - 1];
        return res;
    } /// N * log(N)

    template <typename Type>
    static std::vector <Type> deflatePolynomial(const std::vector <Type>& coefficients, const Type& root) {
        const std::size_t n(coefficients.size() - 1);
        std::vector <Type> newCoefficients(n);
        newCoefficients[0] = coefficients[0];

        for (std::size_t i = 1; i < n; ++i)
            newCoefficients[i] = coefficients[i] + root * newCoefficients[i - 1];

        return newCoefficients;
    } /// N

    template <int EPSILON_ORDER = 9, std::size_t MAX_ITER = 100>
    static VecComp solvePolynomialNewton(const Vec& coefficients) {
        static const long double epsilon(std::pow(10.L, -EPSILON_ORDER));

        auto coeffs(ConvertCore::toComplexVector(coefficients));
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
            root = ConvertCore::normalize(root, epsilon);

        return roots;
    } /// N * log(M)

    /// Решение матрицы
    template <typename Type>
    static std::vector <Type> solveMatrix(Type* matrix, std::size_t n) {
        ///
        for (std::size_t i = 0; i < n; ++i, std::cout << '\n')
            for (std::size_t j = 0; j <= n; ++j)
                std::cout << matrix[i * (n + 1) + j] << ' ';
        ///
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
        ///
        std::vector <Type> answers(n);
        for (std::size_t i = 0; i < n; ++i)
            answers[i] = matrix[i * (n + 1) + n];
        ///
        return answers;
    } /// N^3

    template <class ContainerNumerator, class ContainerRoots, typename Type>
    static ContainerRoots computeFactorsSimpleFractions(const ContainerNumerator& numerator, const ContainerRoots& roots, const Type& highestFactor = 1) {
        std::size_t n(roots.size());
        auto num(numerator);
        num.resize(n, 0);
        Complex matrix[n * n + n];
        for (std::size_t i = 0; i < n; ++i) {
            ContainerRoots x;
            x.reserve(n - 1);
            for (std::size_t j = 0; j < i; ++j)
                x.push_back(roots[j]);
            for (std::size_t j = i + 1; j < n; ++j)
                x.push_back(roots[j]);
            auto coefficients(multiplySimpleFractions(x));
            for (std::size_t j = 0; j < n; ++j)
                matrix[j * (n + 1) + i] = coefficients[j];
        }
        for (std::size_t i = 0; i < n; ++i)
            matrix[i * (n + 1) + n] = num[n - 1 - i] / highestFactor;
        return solveMatrix(matrix, n);
    } /// N^2 * N * log(N) + N^3
};

#endif //REGVALVE_MATH_CORE_HPP

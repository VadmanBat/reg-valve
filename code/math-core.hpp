//
// Created by Vadim on 08.07.2024.
//

#ifndef REGVALVE_MATH_CORE_HPP
#define REGVALVE_MATH_CORE_HPP

#include "convert-core.hpp"

/// Базовый класс с математическими операциями, включает в себя следующее:
///     - формирование диапазонов (постоянный, логарифмический шаг);
///     - вычисление значения полинома в определённой точке схемой Горнера;
///     - перемножении двух полиномов;
///     - перемножение биномов;
///     - дефляция полинома при известном корне методом Горнера;
///     - вычисление корней полинома численным методом Ньютона;
///     - уточнение корней полинома численным методом Ньютона;
///     - решение матрицы аналитическим методом Гаусса-Жордана;
///     - вычисление коэффициентов простейших для разложения рациональной дроби методом Д’Аламбера;
///     - вычисление (a1 * exp(b1) + a2 * exp(b2) + ... + an * exp(bn))^2.

class MathCore {
public:
    using Type      = ProjectTypes::Type;
    using Complex   = ProjectTypes::Complex;

    using Vec       = std::vector <Type>;
    using Pair      = std::pair <Type, Type>;
    using VecPair   = std::vector <std::pair <Type, Type>>;
    using VecComp   = std::vector <Complex>;
    using ValComp   = std::valarray <Complex>;

    template <class ContainerType = Vec>
    static ContainerType range(const Type& min, const Type& max, const std::size_t& points) {
        const Type step((max - min) / static_cast<Type>(points - 1));
        ContainerType res;
        res.reserve(points);
        for (int i = 0; i < points; ++i)
            res.emplace_back(min + i * step);
        return res;
    } /// N
    template <class ContainerType = Vec>
    static ContainerType range(const Pair& interval, const std::size_t& points) {
        const auto[min, max](interval);
        const Type step((max - min) / static_cast<Type>(points - 1));
        ContainerType res;
        res.reserve(points);
        for (int i = 0; i < points; ++i)
            res.emplace_back(min + i * step);
        return res;
    } /// N

    template <class ContainerType = Vec>
    static Vec logRange(const Type& min,const Type& max, const std::size_t& points, bool isFromScratch = false) {
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
    template <class ContainerType = Vec>
    static Vec logRange(const Pair& interval, const std::size_t& points, bool isFromScratch = false) {
        const auto& [min, max](interval);

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

    /// Вычисление полинома при перемножении двух полиномов
    template <typename Container>
    static Container multiply(const Container& a, const Container& b) {
        const std::size_t n = a.size(), m = b.size();
        Container result(n + m - 1, 0);
        for (std::size_t i = 0; i < n; ++i)
            for (std::size_t j = 0; j < m; ++j)
                result[i + j] += a[i] * b[j];
        return result;
    } /// N * M

    /// Вычисление полинома при перемножении биномов (x - r1)(x - r2)
    template <typename Type>
    static std::vector <Type> multiplyBinomials(const std::vector <Type>& coefficients) {
        const std::size_t n(coefficients.size());
        std::vector <Type> res(n + 1);
        res[0] = 1;
        if (n > 0) {
            res[1] = coefficients[0];
            for (std::size_t i = 1; i < n; ++i)
                for (std::size_t j = i + 1; j >= 1; --j)
                    res[j] += coefficients[i] * res[j - 1];
        }
        return res;
    } /// N^2 / 2

    template <typename Type>
    static std::vector <Type> deflatePolynomial(const std::vector <Type>& coefficients, const Type& root) {
        const std::size_t n(coefficients.size() - 1);
        std::vector <Type> newCoefficients(n);
        newCoefficients[0] = coefficients[0];

        for (std::size_t i = 1; i < n; ++i)
            newCoefficients[i] = coefficients[i] + root * newCoefficients[i - 1];

        return newCoefficients;
    } /// N

    template <int EPSILON_ORDER = 6, std::size_t MAX_ITER = 100>
    static VecComp solvePolynomialNewton(const Vec& coefficients) {
        static const Type epsilon(std::pow(10.L, -EPSILON_ORDER));

        auto coeffs(ConvertCore::toComplexVector(coefficients));
        std::size_t degree(coeffs.size() - 1);
        auto f_init = [=](const Complex& x) {
            //Complex result(0);
            std::complex <long double> result(0);
            for (std::size_t i = 0; i <= degree; ++i) {
                result *= x;
                result += coeffs[i];
            }
            return result;
        };
        auto df_init = [=](const Complex& x) {
            //Complex result(0);
            std::complex <long double> result(0);
            for (std::size_t i = 0; i < degree; ++i) {
                result *= x;
                result += Complex(static_cast<Type>(degree - i)) * coeffs[i];
            }
            return result;
        };
        auto f = [&coeffs, &degree](const Complex& x) {
            //Complex result(0);
            std::complex <long double> result(0);
            for (std::size_t i = 0; i <= degree; ++i) {
                result *= x;
                result += coeffs[i];
            }
            return result;
        };
        auto df = [&coeffs, &degree](const Complex& x) {
            //Complex result(0);
            std::complex <long double> result(0);
            for (std::size_t i = 0; i < degree; ++i) {
                result *= x;
                result += Complex(static_cast<Type>(degree - i)) * coeffs[i];
            }
            return result;
        };

        VecComp roots;
        while (degree > 0) {
            Complex x(-1, 1);
            for (std::size_t i = 0; i < MAX_ITER; ++i) {//std::cout << "i = " << i << ", value  = " << std::abs(f(x)) << " | " << std::abs(f_init(x)) << '\n';
                const std::complex <long double> fx(f(x));
                const std::complex <long double> dfx(df(x));
                if (std::abs(fx) < epsilon) {
                    /*x -= f_init(x) / df_init(x);
                    x -= f_init(x) / df_init(x);
                    x -= f_init(x) / df_init(x);*/
                    break;
                }
                x -= fx / dfx;
            }

            if (std::abs(x.imag()) > epsilon) {
                roots.push_back(x);
                coeffs = deflatePolynomial(coeffs, x);
                --degree;
                x.imag(-x.imag());
                /*for (auto k : coeffs)
                    std::cout << k << ' ';
                std::cout << '\n';*/
            }
            roots.push_back(x);
            coeffs = deflatePolynomial(coeffs, x);
            --degree;
            /*for (auto k : coeffs)
                std::cout << k << ' ';
            std::cout << '\n';*/
        }
        if (degree)
            roots.push_back(-coeffs[1] / coeffs[0]);

        /*for (auto& root : roots) {
            std::cout << "root = " << root << '\n';
            std::cout << f_init(root) << ", abs = " << std::abs(f_init(root)) << '\n';
            root = ConvertCore::normalize(root, epsilon);
        }*/

        return roots;
    } /// N * log(log((r - x) / epsilon))
    /*template <int EPSILON_ORDER = 9, std::size_t MAX_ITER = 100>
    static VecComp solve_polynomial_laguerre(const Vec& coefficients) {
        static const Type epsilon(std::pow(10.L, -EPSILON_ORDER));

        std::vector <std::complex <long double>> coeffs(coefficients.size());
        for (int i = 0; i < coefficients.size(); ++i)
            coeffs[i] = coefficients[i];
        //auto coeffs(ConvertCore::toComplexVector(coefficients));
        std::size_t degree(coeffs.size() - 1);
        auto f_init = [=](const std::complex <long double>& x) {
            std::complex <long double> result(0);
            for (std::size_t i = 0; i <= degree; ++i) {
                result *= x;
                result += coeffs[i];
            }
            return result;
        };
        auto f = [&coeffs, &degree](const std::complex <long double>& x) {
            std::complex <long double> result(0);
            for (std::size_t i = 0; i <= degree; ++i) {
                result *= x;
                result += coeffs[i];
            }
            return result;
        };
        auto df = [&coeffs, &degree](const std::complex <long double>& x) {
            std::complex <long double> result(0);
            for (std::size_t i = 0; i < degree; ++i) {
                result *= x;
                result += std::complex <long double>(static_cast<Type>(degree - i)) * coeffs[i];
            }
            return result;
        };
        auto d2f = [&coeffs, &degree](const std::complex <long double>& x) {
            std::complex <long double> result(0);
            int a = degree, b = a - 1;
            for (std::size_t i = 0; i < degree - 1; ++i) {
                result *= x;
                result += std::complex <long double>(a * b) * coeffs[i];
                --a, --b;
            }
            return result;
        };

        VecComp roots;
        while (degree > 0) {
            std::complex <long double> x(-1, 1);
            for (std::size_t i = 0; i < MAX_ITER; ++i) {std::cout << "i = " << i << ", value  = " << std::abs(f(x)) << '\n';
                const std::complex <long double> fx(f(x));
                const std::complex <long double> d1fx(df(x));
                const std::complex <long double> d2fx(d2f(x));
                if (std::abs(fx) < epsilon) {
                    x -= f_init(x) / df_init(x);
                    x -= f_init(x) / df_init(x);
                    x -= f_init(x) / df_init(x);
                    break;
                }

                std::complex <long double> discriminant = std::pow(std::complex <long double>(degree - 1.0) * d1fx, 2.0) - std::complex <long double>(degree * (degree-1.0),0.0)*fx*d2fx;
                std::complex <long double> sqrt_discriminant = sqrt(discriminant);
                std::complex <long double> denominator_1 = d1fx + sqrt_discriminant;
                std::complex <long double> denominator_2 = d1fx - sqrt_discriminant;
                std::complex <long double> denominator = abs(denominator_1) > abs(denominator_2) ? denominator_1 : denominator_2;

                x -= (std::complex <long double>(degree,0.0) * fx) / denominator;
            }

            if (std::abs(x.imag()) > epsilon) {
                roots.emplace_back(x);
                coeffs = deflatePolynomial(coeffs, x);
                --degree;
                x.imag(-x.imag());
                for (auto k : coeffs)
                    std::cout << k << ' ';
                std::cout << '\n';
            }
            roots.emplace_back(x);
            coeffs = deflate_polynomial(coeffs, x);
            --degree;
            for (auto k : coeffs)
                std::cout << k << ' ';
            std::cout << '\n';
        }
        if (degree)
            roots.emplace_back(-coeffs[1] / coeffs[0]);

        for (auto& root : roots) {
            std::cout << "root = " << root << '\n';
            std::cout << f_init(root) << ", abs = " << std::abs(f_init(root)) << '\n';
            root = ConvertCore::normalize(root, epsilon);
        }

        return roots;
    } /// N * log(log((r - x) / epsilon))*/
    template <int EPSILON_ORDER = 9, std::size_t MAX_ITER = 100>
    static VecComp clarifyPolynomialNewton(const Vec& coefficients, VecComp& roots) {
        static const Type epsilon(std::pow(10.L, -EPSILON_ORDER));

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
                result += Complex(static_cast<Type>(degree - i)) * coeffs[i];
            }
            return result;
        };

        for (auto& root : roots)
            for (std::size_t i = 0; i < MAX_ITER; ++i) {
                const Complex fx(f(root));
                const Complex dfx(df(root));
                if (std::abs(fx) < epsilon)
                    break;
                root -= fx / dfx;
            }

        return roots;
    } /// N * log(log((r - x) / epsilon))

    /// Решение матрицы
    template <typename Type>
    static std::vector <Type> solveMatrix(Type* matrix, std::size_t n) {
        const std::size_t cols(n + 1);
        for (std::size_t i = 0; i < n; ++i) {
            Type diagElement(matrix[i * cols + i]);
            for (std::size_t j = 0; j < cols; ++j)
                matrix[i * cols + j] /= diagElement;

            for (std::size_t k = 0; k < i; ++k) {
                Type factor(matrix[k * cols + i]);
                for (std::size_t j = 0; j < cols; ++j)
                    matrix[k * cols + j] -= factor * matrix[i * cols + j];
            }
            for (std::size_t k = i + 1; k < n; ++k) {
                Type factor(matrix[k * cols + i]);
                for (std::size_t j = 0; j < cols; ++j)
                    matrix[k * cols + j] -= factor * matrix[i * cols + j];
            }
        }
        std::vector <Type> answers(n);
        for (std::size_t i = 0; i < n; ++i)
            answers[i] = matrix[i * cols + n];
        return answers;
    } /// N^3

    template <class ContainerNumerator, class ContainerRoots, typename Type>
    static ContainerRoots decomposeFraction(const ContainerNumerator& numerator, const ContainerRoots& roots, const Type& highestFactor = 1) {
        const std::size_t n(roots.size()), cols(n + 1);
        Complex matrix[n * n + n];
        for (std::size_t i = 0; i < n; ++i) {
            ContainerRoots x;
            x.reserve(n - 1);
            for (std::size_t j = 0; j < i; ++j)
                x.push_back(-roots[j]);
            for (std::size_t j = i + 1; j < n; ++j)
                x.push_back(-roots[j]);
            auto coefficients(multiplyBinomials(x));
            for (std::size_t j = 0; j < n; ++j)
                matrix[j * cols + i] = coefficients[j];
        }
        for (std::size_t i = n - std::min(n, numerator.size()), j = 0; i < n; ++i, ++j)
            matrix[i * cols + n] = numerator[j] / highestFactor;
        return solveMatrix(matrix, n);
    } /// N^2 + N^2 + N^3

    template <class Container>
    static std::pair <Container, Container> squareExponentialSum(const Container& coefficients, const Container& exponents) {
        const auto n(std::min(coefficients.size(), exponents.size()));
        Container newCoefficients, newExponents;
        newCoefficients.reserve(n * n);
        newExponents.reserve(n * n);
        for (std::size_t i = 0; i < n; ++i)
            for (std::size_t j = 0; j < n; ++j) {
                newCoefficients.emplace_back(coefficients[i] * coefficients[j]);
                newExponents.emplace_back(exponents[i] + exponents[j]);
            }
        return {newCoefficients, newExponents};
    } /// 2 * N^2
};

#endif //REGVALVE_MATH_CORE_HPP

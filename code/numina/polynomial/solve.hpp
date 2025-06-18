#ifndef MATH_CORE_SOLVE_POLYNOMIAL_HPP
#define MATH_CORE_SOLVE_POLYNOMIAL_HPP

#include "deflate.hpp"

namespace numina {
    template <typename Type>
    static std::vector <std::complex <Type>> solve_polynomial_laguerre(const std::vector <Type>& coeffs) {
        static const Type epsilon = std::sqrt(std::numeric_limits<Type>::epsilon());

        std::size_t degree = coeffs.size() - 1;
        Type a[degree + 1], a_d1[degree], a_d2[degree];

        std::copy(coeffs.begin(), coeffs.end(), a);
        int power = static_cast<int>(degree);
        for (std::size_t i = 0; i < degree; ++i) {
            a_d1[i] = power * a[i];
            a_d2[i] = --power * a_d1[i];
        }

        auto deflate = [&a, &a_d1, &a_d2, &degree](const Type& root) {
            --degree;
            for (std::size_t i = 1; i <= degree; ++i)
                a[i] += root * a[i - 1];

            int power = static_cast<int>(degree);
            for (std::size_t i = 0; i < degree; ++i) {
                a_d1[i] = power * a[i];
                a_d2[i] = --power * a_d1[i];
            }
        };
        auto deflate_conj = [&a, &a_d1, &a_d2, &degree](const std::complex <Type>& root) {
            const Type real = root.real();
            const Type imag = root.imag();
            const Type c1 = 2 * real;
            const Type c2 = real * real + imag * imag;

            degree -= 2;
            a[1] += c1 * a[0];
            for (std::size_t i = 2; i <= degree; ++i)
                a[i] += c1 * a[i - 1] - c2 * a[i - 2];

            int power = static_cast<int>(degree);
            for (std::size_t i = 0; i < degree; ++i) {
                a_d1[i] = power * a[i];
                a_d2[i] = --power * a_d1[i];
            }
        };

        auto f = [&a, &degree](const std::complex <Type>& x) {
            std::complex <Type> result = a[0];
            for (std::size_t i = 1; i <= degree; ++i) {
                result *= x;
                result += a[i];
            }
            return result;
        };
        auto d1f = [&a_d1, &degree](const std::complex <Type>& x) {
            std::complex <Type> result = a_d1[0];
            for (std::size_t i = 1; i < degree; ++i) {
                result *= x;
                result += a_d1[i];
            }
            return result;
        };
        auto d2f = [&a_d2, &degree](const std::complex <Type>& x) {
            std::complex <Type> result = a_d2[0];
            for (std::size_t i = 1; i < degree - 1; ++i) {
                result *= x;
                result += a_d2[i];
            }
            return result;
        };

        /*auto f = [&a, &degree](const std::complex <Type>& x) {
            std::complex <Type> result = 0;
            for (std::size_t i = 0; i <= degree; ++i) {
                result *= x;
                result += a[i];
            }
            return result;
        };
        auto d1f = [&coeffs, &degree](const std::complex <Type>& x) {
            std::complex <Type> result(0);
            int a = static_cast<int>(degree);
            for (std::size_t i = 0; i < degree; ++i) {
                result *= x;
                result += a * coeffs[i];
                --a;
            }
            return result;
        };
        auto d2f = [&coeffs, &degree](const std::complex <Type>& x) {
            std::complex <Type> result(0);
            int a = static_cast<int>(degree);
            for (std::size_t i = 0; i < degree - 1; ++i) {
                result *= x;
                result += a * (a - 1) * coeffs[i];
                --a;
            }
            return result;
        };*/

        std::vector <std::complex <Type>> roots;
        roots.reserve(degree);
        while (degree > 1) {
            std::complex <Type> x(-1, 1), root;
            Type error = 1, current_error;
            for (std::size_t i = 0; i < 100; ++i) {
                const auto fx = f(x);
                current_error = std::abs(fx);
                if (current_error >= error && error < 1e-1)
                    break;
                error = current_error;

                const auto d1fx = d1f(x);
                const auto d2fx = d2f(x);
                const auto b = std::complex <Type>(degree - 1) * d1fx;
                const auto discriminant = std::sqrt(b * b - std::complex <Type>(degree * (degree - 1)) * fx * d2fx);
                const auto a1 = d1fx - discriminant;
                const auto a2 = d1fx + discriminant;

                root = x;
                x -= (std::complex <Type>(degree) * fx) / (std::abs(a1) > std::abs(a2) ? a1 : a2);
            }

            if (std::abs(root.imag()) > epsilon) {
                roots.emplace_back(std::conj(root));
                roots.emplace_back(root);
                //coeffs = deflate_polynomial_conj_root(coeffs, root);
                deflate_conj(root);
                //degree -= 2;
            }
            else {
                const auto value = root.real(); roots.emplace_back(value);
                //coeffs = deflate_polynomial(coeffs, value);
                deflate(value);
                //--degree;
            }
        }
        if (degree)
            roots.emplace_back(-coeffs[1] / coeffs[0]);

        return roots;
    } /// N * log(log((r - x) / epsilon))
}

#endif //MATH_CORE_SOLVE_POLYNOMIAL_HPP

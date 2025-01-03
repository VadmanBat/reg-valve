//
// Created by Vadma on 03.01.2025.
//

#ifndef REGVALVE_SOLVE_POLYNOMIAL_HPP
#define REGVALVE_SOLVE_POLYNOMIAL_HPP

#include "deflate-polynomial.hpp"

namespace SupMathCore {
    template <typename Type>
    static std::vector <std::complex <Type>> solvePolynomialLaguerre(std::vector <Type> coefficients) {
        static const Type epsilon = std::sqrt(std::numeric_limits<double>::epsilon());
        std::size_t degree(coefficients.size() - 1);

        auto f = [&coefficients, &degree](const std::complex <Type>& x) {
            std::complex <Type> result(0);
            for (std::size_t i = 0; i <= degree; ++i) {
                result *= x;
                result += coefficients[i];
            }
            return result;
        };
        auto d1f = [&coefficients, &degree](const std::complex <Type>& x) {
            std::complex <Type> result(0);
            int a = static_cast<int>(degree);
            for (std::size_t i = 0; i < degree; ++i) {
                result *= x;
                result += a * coefficients[i];
                --a;
            }
            return result;
        };
        auto d2f = [&coefficients, &degree](const std::complex <Type>& x) {
            std::complex <Type> result(0);
            int a = static_cast<int>(degree);
            for (std::size_t i = 0; i < degree - 1; ++i) {
                result *= x;
                result += a * (a - 1) * coefficients[i];
                --a;
            }
            return result;
        };

        std::vector <std::complex <Type>> roots;
        roots.reserve(degree);
        while (degree > 1) {
            std::complex <Type> x(-1, 1), root;
            Type error = 1, currentError;
            for (std::size_t i = 0; i < 100; ++i) {
                const auto fx = f(x);
                currentError = std::abs(fx);
                if (currentError >= error && error < 1e-1)
                    break;
                error = currentError;

                const auto d1fx = d1f(x);
                const auto d2fx = d2f(x);
                const auto b = std::complex <Type>(degree - 1) * d1fx;
                const auto discriminant = std::sqrt(b * b - std::complex <Type>(degree * (degree - 1)) * fx * d2fx);
                const auto a1 = d1fx + discriminant;
                const auto a2 = d1fx - discriminant;

                root = x;
                x -= (std::complex <Type>(degree) * fx) / (std::abs(a1) > std::abs(a2) ? a1 : a2);
            }

            if (std::abs(root.imag()) > epsilon) {
                roots.emplace_back(std::conj(root));
                roots.emplace_back(root);
                coefficients = deflatePolynomialConjRoot(coefficients, root);
                degree -= 2;
            }
            else {
                const auto value = root.real(); roots.emplace_back(value);
                coefficients = deflatePolynomial(coefficients, value);
                --degree;
            }
        }
        if (degree)
            roots.emplace_back(-coefficients[1] / coefficients[0]);

        return roots;
    } /// N * log(log((r - x) / epsilon))
}

#endif //REGVALVE_SOLVE_POLYNOMIAL_HPP

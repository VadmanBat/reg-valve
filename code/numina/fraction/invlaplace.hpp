//
// Created by Vadim on 16.01.2025.
//

#ifndef REGVALVE_INVLAPLACE_HPP
#define REGVALVE_INVLAPLACE_HPP

#include "parfrac.hpp"
#include "../terms/term-expression.hpp"

namespace numina {
    template <class Type>
    static TermExpression <Type> invlaplace(const std::vector <Type>& num, const std::vector <Type>& den, std::vector <std::complex <Type>> roots) {
        static const Type epsilon = std::sqrt(std::numeric_limits<Type>::epsilon());
        const auto coefficients = ConvertCore::toComplexVector(den);
        const std::size_t n(roots.size()), cols(n + 1);

        std::sort(roots.begin(), roots.end(),
                  [](const auto& a, const auto& b){
                      return a.real() != b.real() ? a.real() > b.real() : a.imag() < b.imag();
                  });

        std::vector <int> powers(n, 0);
        std::vector <std::complex <Type>> vec_coeffs[n];
        vec_coeffs[0] = deflate_polynomial(coefficients, roots[0]);
        for (std::size_t i = 1; i < n; ++i) {
            if (std::abs(roots[i - 1] - roots[i]) < epsilon) {
                powers[i] = powers[i - 1] + 1;
                vec_coeffs[i] = deflate_polynomial(vec_coeffs[i - 1], roots[i]);
                continue;
            }
            vec_coeffs[i] = deflate_polynomial(coefficients, roots[i]);
        }

        std::complex <Type> matrix[n * n + n] = {};
        for (std::size_t i = 0; i < n; ++i) {
            const auto& coeffs = vec_coeffs[i];
            const auto m = coeffs.size(), del = n - m;
            for (std::size_t j = 0; j < m; ++j)
                matrix[(j + del) * cols + i] = coeffs[j];
        }
        const auto& higFactor = den[0];
        for (std::size_t i = n - std::min(n, num.size()), j = 0; i < n; ++i, ++j)
            matrix[i * cols + n] = num[j] / higFactor;

        auto factors = solve_matrix_gauss(matrix, n);
        Type factorial = 1;
        for (std::size_t i = 1; i < n; ++i) {
            if (powers[i] != 0) {
                factorial *= powers[i];
                factors[i] /= factorial;
                continue;
            }
            factorial = 1;
        }
        return TermExpression <Type>(roots, factors, powers);
    } /// N^2 + N^2 + N^3
}


#endif //REGVALVE_INVLAPLACE_HPP

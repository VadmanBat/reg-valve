//
// Created by Vadma on 04.01.2025.
//

#ifndef MATH_CORE_DECOMPOSE_FRACTION_HPP
#define MATH_CORE_DECOMPOSE_FRACTION_HPP

#include <vector>
#include <complex>

#include "../polynomial/multiply.hpp"
#include "../polynomial/deflate.hpp"
#include "../matrix/solve.hpp"

namespace SupMathCore {
    template <class ContainerNumerator, class ContainerRoots, typename Type>
    static ContainerRoots decomposeFraction(const ContainerNumerator& numerator, const ContainerRoots& roots, const Type& highestFactor = 1) {
        const std::size_t n(roots.size()), cols(n + 1);
        std::complex <Type> matrix[n * n + n];
        const auto coefficients(multiplyBinomials(roots));
        for (std::size_t i = 0; i < n; ++i) {
            const auto coeffs = deflatePolynomial(coefficients, roots[i]);
            for (std::size_t j = 0; j < n; ++j)
                matrix[j * cols + i] = coeffs[j];
        }
        for (std::size_t i = n - std::min(n, numerator.size()), j = 0; i < n; ++i, ++j)
            matrix[i * cols + n] = numerator[j] / highestFactor;
        return solveMatrixGauss(matrix, n);
    } /// N^2 + N^2 + N^3
}

#endif //MATH_CORE_DECOMPOSE_FRACTION_HPP

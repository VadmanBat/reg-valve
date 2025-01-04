#ifndef MATH_CORE_DEFLATE_POLYNOMIAL_HPP
#define MATH_CORE_DEFLATE_POLYNOMIAL_HPP

#include <vector>
#include <complex>

namespace SupMathCore {
    template <typename Type>
    static std::vector <Type> deflatePolynomial(const std::vector <Type>& coefficients, const Type& root) {
        const std::size_t n(coefficients.size() - 1);
        std::vector <Type> newCoefficients(n);
        newCoefficients[0] = coefficients[0];

        for (std::size_t i = 1; i < n; ++i)
            newCoefficients[i] = coefficients[i] + root * newCoefficients[i - 1];

        return newCoefficients;
    } /// N

    template <typename Type, typename RootType>
    static std::vector <Type> deflatePolynomialConjRoot(const std::vector <Type>& coefficients, const RootType& root) {
        const std::size_t n = coefficients.size() - 2;
        if (n < 2)
            return {};

        const Type a = root.real();
        const Type b = root.imag();
        const Type c1 = 2 * a;
        const Type c2 = a * a + b * b;

        std::vector <Type> newCoefficients(n);
        newCoefficients[0] = coefficients[0];
        newCoefficients[1] = coefficients[1] + c1 * newCoefficients[0];

        for (size_t i = 2; i < n; ++i)
            newCoefficients[i] = coefficients[i] + c1 * newCoefficients[i - 1] - c2 * newCoefficients[i - 2];

        return newCoefficients;
    }
}

#endif //MATH_CORE_DEFLATE_POLYNOMIAL_HPP

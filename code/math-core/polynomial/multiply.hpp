#ifndef MATH_CORE_MULTIPLY_POLYNOMIAL_HPP
#define MATH_CORE_MULTIPLY_POLYNOMIAL_HPP

#include <vector>

namespace SupMathCore {
    /// Вычисление значения полинома по схеме Горнера
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
    static std::vector <Type> multiplyBinomials(const std::vector <Type>& roots) {
        const std::size_t n = roots.size();
        std::vector <Type> res(n + 1);
        res[0] = 1;
        if (n > 0) {
            res[1] = -roots[0];
            for (std::size_t i = 1; i < n; ++i)
                for (std::size_t j = i + 1; j >= 1; --j)
                    res[j] -= roots[i] * res[j - 1];
        }
        return res;
    } /// N^2 / 2
}

#endif //MATH_CORE_MULTIPLY_POLYNOMIAL_HPP
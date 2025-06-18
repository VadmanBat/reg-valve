#ifndef MATH_CORE_MULTIPLY_POLYNOMIAL_HPP
#define MATH_CORE_MULTIPLY_POLYNOMIAL_HPP

#include <vector>

namespace numina {
    /// Вычисление значения полинома в определённой по схеме Горнера
    template <typename Type>
    inline static Type polynomial(const std::vector <Type>& coeffs, const Type& x) {
        Type result = 0;
        const auto n = coeffs.size();
        const auto a = coeffs.data();
        for (std::size_t i = 0; i < n; ++i) {
            result *= x;
            result += a[i];
        }
        return result;
    } /// N

    /// Вычисление полинома при перемножении двух полиномов
    template <typename Container>
    inline static Container multiply(const Container& a, const Container& b) {
        const auto n = a.size();
        const auto m = b.size();
        Container result(n + m - 1, 0);

        const auto x = a.data();
        const auto y = b.data();
        const auto z = result.data();

        for (std::size_t i = 0; i < n; ++i)
            for (std::size_t j = 0; j < m; ++j)
                z[i + j] += x[i] * y[j];

        return result;
    } /// N * M

    /// Вычисление полинома при перемножении биномов (x - r1)(x - r2)
    template <typename Type>
    inline static std::vector <Type> multiply_binomials(const std::vector <Type>& roots) {
        const auto n = roots.size();
        std::vector <Type> answer(n + 1);

        const auto x = answer.data();
        const auto a = roots.data();
        answer[0] = 1;
        if (n > 0) {
            x[1] = -a[0];
            for (std::size_t i = 1; i < n; ++i)
                for (std::size_t j = i + 1; j >= 1; --j)
                    x[j] -= a[i] * x[j - 1];
        }
        return answer;
    } /// N^2 / 2
}

#endif //MATH_CORE_MULTIPLY_POLYNOMIAL_HPP
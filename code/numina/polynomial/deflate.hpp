//module;

#ifndef NUMINA_POLYNOMIAL_DEFLATE_HPP
#define NUMINA_POLYNOMIAL_DEFLATE_HPP

#include <vector>
#include <complex>

//export module numina.deflate_polynomial;

namespace numina {
    template <typename Type>
    std::vector <Type> deflate_polynomial(const std::vector <Type>& coeffs, const Type& root) {
        const auto n = coeffs.size() - 1;
        std::vector <Type> answer(n);

        const auto a = answer.data();
        const auto c = coeffs.data();

        a[0] = c[0];
        for (std::size_t i = 1; i < n; ++i)
            a[i] = c[i] + root * a[i - 1];

        return answer;
    } /// N

    template <typename Type, typename RootType>
    std::vector <Type> deflate_polynomial_conj_root(const std::vector <Type>& coeffs, const RootType& root) {
        const auto n = coeffs.size() - 2;
        if (n < 2)
            return {};

        const Type real = root.real();
        const Type imag = root.imag();
        const Type c1 = 2 * real;
        const Type c2 = real * real + imag * imag;

        std::vector <Type> answer(n);
        const auto a = answer.data();
        const auto c = coeffs.data();

        a[0] = c[0];
        a[1] = c[1] + c1 * a[0];
        for (std::size_t i = 2; i < n; ++i)
            a[i] = c[i] + c1 * a[i - 1] - c2 * a[i - 2];

        return answer;
    }
}

#endif

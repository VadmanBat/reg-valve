//
// Created by Vadim on 18.06.2025.
//

#ifndef REGVALVE_POLYNOMIAL_HPP
#define REGVALVE_POLYNOMIAL_HPP

#include <vector>
#include <complex>

template <typename Type>
class Polynomial {
private:
    std::vector <Type> coeffs;

    std::size_t n;
    Type* c;

public:
    Polynomial(const std::vector <Type>& coefficients) :
            coeffs(coefficients),
            n(coeffs.size()),
            c(coeffs.data())
    {

    }
    Polynomial(std::vector <Type>&& coefficients) :
            coeffs(std::move(coefficients)),
            n(coeffs.size()),
            c(coeffs.data())
    {

    }

    inline Type operator()(const Type& x) const {
        if (n == 0)
            return Type(0);

        Type result = c[0];
        for (std::size_t i = 1; i < n; ++i) {
            result *= x;
            result += c[i];
        }
        return result;
    }

    inline Polynomial derivative() const {
        if (n == 0)
            return Polynomial();

        std::vector <Type> new_coeffs(n - 1);
        auto new_c = new_coeffs.data();
        int power = static_cast<int>(n);
        for (size_t i = 0; i < n - 1; ++i)
            new_c[i] = --power * c[i];

        return Polynomial(new_coeffs);
    }

    inline Polynomial integral(const Type& constant = Type(0)) const {
        std::vector <Type> new_coeffs(n + 1);
        auto new_c = new_coeffs.data();
        int power = static_cast<int>(n);
        for (size_t i = 0; i < n; ++i) {
            new_c[i] = c[i] / power;
            --power;
        }
        new_c[n] = constant;
        return Polynomial(new_coeffs);
    }

    void makeDerivative() {
        if (n == 0)
            return;

        int power = static_cast<int>(n);
        --n;
        for (size_t i = 0; i < n; ++i)
            c[i] = --power * c[i];
        coeffs.pop_back();
    }

    void makeIntegral(const Type& constant = Type(0)) {
        int power = static_cast<int>(n);
        for (size_t i = 0; i < n; ++i) {
            c[i] = c[i] / power;
            --power;
        }
        ++n;
        coeffs.push_back(constant);
        c = coeffs.data();
    }

    inline int degree() const {
        return n == 0 ? -1 : static_cast<int>(n - 1);
    }

    inline Type operator[](std::size_t index) const {
        return c[index];
    }

    inline bool operator==(const Polynomial& other) const {
        if (n != other.n)
            return false;
        for (std::size_t i = 0; i < n; ++i)
            if (c[i] != other.c[i])
                return false;
        return true;
    }

    inline bool operator!=(const Polynomial& other) const {
        if (n != other.n)
            return true;
        for (std::size_t i = 0; i < n; ++i)
            if (c[i] != other.c[i])
                return true;
        return false;
    }

    Polynomial deflate(Type root) const {
        if (n < 1)
            return {};

        const auto new_n = n - 1;
        std::vector <Type> answer(new_n);
        const auto a = answer.data();

        a[0] = c[0];
        for (std::size_t i = 1; i < new_n; ++i)
            a[i] = c[i] + root * a[i - 1];

        return Polynomial(answer);
    }

    Polynomial deflateConjRoot(std::complex <Type> root) const {
        if (n < 2)
            return {};

        const auto new_n = n - 2;

        const Type real = root.real();
        const Type imag = root.imag();
        const Type c1 = 2 * real;
        const Type c2 = real * real + imag * imag;

        std::vector <Type> answer(new_n);
        const auto a = answer.data();

        a[0] = c[0];
        a[1] = c[1] + c1 * a[0];
        for (std::size_t i = 2; i < new_n; ++i)
            a[i] = c[i] + c1 * a[i - 1] - c2 * a[i - 2];

        return Polynomial(answer);
    }
};

#endif //REGVALVE_POLYNOMIAL_HPP

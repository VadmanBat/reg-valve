//
// Created by Vadim on 05.01.2025.
//

#ifndef MATH_CORE_TERM_HPP
#define MATH_CORE_TERM_HPP

#include <sstream>
#include <iomanip>
#include <complex>
#include <vector>

template <typename Type>
class Term {
public:
    virtual ~Term() = default;
    [[nodiscard]] virtual inline Type value() const = 0;
    [[nodiscard]] virtual inline std::vector <Term*> derivative() const = 0;
    [[nodiscard]] virtual inline Term* clone() const = 0;

    [[nodiscard]] virtual inline bool isPositive() const = 0;
    [[nodiscard]] virtual inline std::string string() const = 0;
    [[nodiscard]] virtual inline std::string unsignedString() const = 0;

    [[nodiscard]] virtual inline Type derivativeConstant() const {
        return 0;
    };

    inline static Type time;
    inline static void setPrecision(int n) {
        std::stringstream() << std::fixed << std::setprecision(n);
    }
};

template <typename Type>
class ExpTerm : public Term <Type> {
private:
    const Type coefficient, root;

public:
    explicit ExpTerm(Type c, Type r) :
            coefficient(c),
            root(r)
    {

    }

    ExpTerm(const ExpTerm& other) :
            coefficient(other.coefficient),
            root(other.root)
    {

    }

    inline Term <Type>* clone() const override {
        return new ExpTerm(*this);
    }

    inline Type value() const override {
        return coefficient * std::exp(root * Term<Type>::time);
    }

    [[nodiscard]] inline std::vector <Term <Type>*> derivative() const override {
        return {new ExpTerm(coefficient * root, root)};
    }

    [[nodiscard]] bool isPositive() const override {
        return coefficient > 0;
    }

    [[nodiscard]] inline std::string string() const override {
        return (std::stringstream() << coefficient << " × e<sup>" << root << " × t</sup>").str();
    }

    [[nodiscard]] inline std::string unsignedString() const override {
        return (std::stringstream() << std::abs(coefficient) << " × e<sup>" << root << " × t</sup>").str();
    }
};

template <typename Type>
class CosTerm : public Term <Type> {
private:
    using Comp = std::complex <Type>;
    const Type amplitude, omega, phi;

public:
    explicit CosTerm(const Comp& c, const Comp& r) :
            amplitude(2 * std::abs(c)),
            omega(r.imag()),
            phi(std::atan2(c.imag(), c.real()))
    {

    }

    explicit CosTerm(Type amplitude, Type omega, Type phi) :
            amplitude(amplitude),
            omega(omega),
            phi(std::remainder(phi, 2 * std::numbers::pi_v<Type>))
    {

    }

    CosTerm(const CosTerm& other) :
            amplitude(other.amplitude),
            omega(other.omega),
            phi(other.phi)
    {

    }

    inline Term <Type>* clone() const override {
        return new CosTerm(*this);
    }

    inline Type value() const override {
        return amplitude * std::cos(omega * Term<Type>::time + phi);
    }

    [[nodiscard]] inline std::vector <Term <Type>*> derivative() const override {
        return {new CosTerm(-amplitude * omega, omega, phi - std::numbers::pi_v<Type> / 2)};
    }

    [[nodiscard]] bool isPositive() const override {
        return amplitude > 0;
    }

    [[nodiscard]] inline std::string string() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × cos(" << omega << " × t)").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << ')').str();
    }

    [[nodiscard]] inline std::string unsignedString() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × cos(" << omega << " × t)").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << ')').str();
    }
};

template <typename Type>
class ExpCosTerm : public Term <Type> {
private:
    using Comp = std::complex <Type>;
    const Type amplitude, alpha, omega, phi;

public:
    explicit ExpCosTerm(const Comp& c, const Comp& r) :
            amplitude(2 * std::abs(c)),
            alpha(r.real()),
            omega(r.imag()),
            phi(std::atan2(c.imag(), c.real()))
    {

    }

    explicit ExpCosTerm(Type amplitude, Type alpha, Type omega, Type phi) :
            amplitude(amplitude),
            alpha(alpha),
            omega(omega),
            phi(std::remainder(phi, 2 * std::numbers::pi_v<Type>))
    {

    }

    ExpCosTerm(const ExpCosTerm& other) :
            amplitude(other.amplitude),
            alpha(other.alpha),
            omega(other.omega),
            phi(other.phi)
    {

    }

    inline Term <Type>* clone() const override {
        return new ExpCosTerm(*this);
    }

    inline Type value() const override {
        return amplitude * std::exp(alpha * Term<Type>::time) * std::cos(omega * Term<Type>::time + phi);
    }

    [[nodiscard]] inline std::vector <Term <Type>*> derivative() const override {
        return {
            new ExpCosTerm(amplitude * alpha, alpha, omega, phi),
            new ExpCosTerm(-omega * amplitude, alpha, omega, phi - std::numbers::pi_v<Type> / 2)
        };
    }

    [[nodiscard]] bool isPositive() const override {
        return amplitude > 0;
    }

    [[nodiscard]] inline std::string string() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t)").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << ')').str();
    }

    [[nodiscard]] inline std::string unsignedString() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t)").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << ')').str();
    }
};

#endif //MATH_CORE_TERM_HPP

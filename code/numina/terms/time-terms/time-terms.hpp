//
// Created by Vadim on 07.01.2025.
//

#ifndef MATH_CORE_TIME_TERMS_HPP
#define MATH_CORE_TIME_TERMS_HPP

#include "../term.hpp"

template <typename Type>
class TimeTerm : public Term <Type> {
private:
    const Type coefficient;

public:
    explicit TimeTerm(Type a) :
            coefficient(a)
    {

    }

    TimeTerm(const TimeTerm& other) :
            coefficient(other.coefficient)
    {

    }

    inline Term <Type>* clone() const override {
        return new TimeTerm(*this);
    }

    inline Type value() const override {
        return coefficient * Term<Type>::time;
    }

    [[nodiscard]] inline std::vector <Term <Type>*> derivative() const override {
        return {};
    }

    [[nodiscard]] bool isPositive() const override {
        return coefficient > 0;
    }

    [[nodiscard]] inline std::string string() const override {
        return (std::stringstream() << coefficient << " × t").str();
    }

    [[nodiscard]] inline std::string unsignedString() const override {
        return (std::stringstream() << std::abs(coefficient) << " × t").str();
    }

    [[nodiscard]] inline Type derivativeConstant() const override {
        return coefficient;
    };
};

template <typename Type>
class ExpTimeTerm : public Term <Type> {
private:
    Type coefficient, root;

public:
    explicit ExpTimeTerm(Type c, Type r) :
            coefficient(c),
            root(r)
    {

    }

    ExpTimeTerm(const ExpTimeTerm& other) :
            coefficient(other.coefficient),
            root(other.root)
    {

    }

    inline Term <Type>* clone() const override {
        return new ExpTimeTerm(*this);
    }

    inline Type value() const override {
        return coefficient * std::exp(root * Term<Type>::time) * Term<Type>::time;
    }

    inline std::vector <Term <Type>*> derivative() const override {
        return {
            new ExpTerm(coefficient * root, root),
            new ExpTerm(coefficient, root)
        };
    }

    [[nodiscard]] bool isPositive() const override {
        return coefficient > 0;
    }

    [[nodiscard]] inline std::string string() const override {
        return (std::stringstream() << coefficient << " × e<sup>" << root << " × t</sup> × t").str();
    }

    [[nodiscard]] inline std::string unsignedString() const override {
        return (std::stringstream() << std::abs(coefficient) << " × e<sup>" << root << " × t</sup> × t").str();
    }
};

template <typename Type>
class CosTimeTerm : public Term <Type> {
private:
    using Comp = std::complex <Type>;
    const Type amplitude, omega, phi;

public:
    explicit CosTimeTerm(const Comp& c, const Comp& r) :
            amplitude(2 * std::abs(c)),
            omega(r.imag()),
            phi(std::atan2(c.imag(), c.real()))
    {

    }

    explicit CosTimeTerm(Type amplitude, Type omega, Type phi) :
            amplitude(amplitude),
            omega(omega),
            phi(std::remainder(phi, 2 * std::numbers::pi_v<Type>))
    {

    }

    CosTimeTerm(const CosTimeTerm& other) :
            amplitude(other.amplitude),
            omega(other.omega),
            phi(other.phi)
    {

    }

    inline Term <Type>* clone() const override {
        return new CosTimeTerm(*this);
    }

    inline Type value() const override {
        return amplitude * std::cos(omega * Term<Type>::time + phi) * Term<Type>::time;
    }

    [[nodiscard]] inline std::vector <Term <Type>*> derivative() const override {
        return {
            new CosTerm(amplitude, omega, phi),
            new CosTimeTerm(-amplitude * omega, omega, phi - std::numbers::pi_v<Type> / 2)
        };
    }

    [[nodiscard]] bool isPositive() const override {
        return amplitude > 0;
    }

    [[nodiscard]] inline std::string string() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × cos(" << omega << " × t) × t").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << ") × t").str();
    }

    [[nodiscard]] inline std::string unsignedString() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × cos(" << omega << " × t) × t").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << ") × t").str();
    }
};

template <typename Type>
class ExpCosTimeTerm : public Term <Type> {
private:
    using Comp = std::complex <Type>;
    const Type amplitude, alpha, omega, phi;

public:
    explicit ExpCosTimeTerm(const Comp& c, const Comp& r) :
            amplitude(2 * std::abs(c)),
            alpha(r.real()),
            omega(r.imag()),
            phi(std::atan2(c.imag(), c.real()))
    {

    }

    explicit ExpCosTimeTerm(Type amplitude, Type alpha, Type omega, Type phi) :
            amplitude(amplitude),
            alpha(alpha),
            omega(omega),
            phi(std::remainder(phi, 2 * std::numbers::pi_v<Type>))
    {

    }

    ExpCosTimeTerm(const ExpCosTimeTerm& other) :
            amplitude(other.amplitude),
            alpha(other.alpha),
            omega(other.omega),
            phi(other.phi)
    {

    }

    inline Term <Type>* clone() const override {
        return new ExpCosTimeTerm(*this);
    }

    inline Type value() const override {
        return amplitude * std::exp(alpha * Term<Type>::time) * std::cos(omega * Term<Type>::time + phi) * Term<Type>::time;
    }

    [[nodiscard]] inline std::vector <Term <Type>*> derivative() const override {
        return {
                new ExpCosTerm(amplitude, alpha, omega, phi),
                new ExpCosTimeTerm(amplitude, alpha, omega, phi),
                new ExpCosTimeTerm(-amplitude * omega, alpha, omega, phi - std::numbers::pi_v<Type> / 2)
        };
    }

    [[nodiscard]] bool isPositive() const override {
        return amplitude > 0;
    }

    [[nodiscard]] inline std::string string() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t) × t").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << ") × t").str();
    }

    [[nodiscard]] inline std::string unsignedString() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t) × t").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << ") × t").str();
    }
};

#endif //MATH_CORE_TIME_TERMS_HPP

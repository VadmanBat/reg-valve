//
// Created by Vadim on 07.01.2025.
//

#ifndef MATH_CORE_UTIME_TERMS_HPP
#define MATH_CORE_UTIME_TERMS_HPP

#include "time-terms.hpp"

template <typename Type>
class UTimeTerm : public Term <Type> {
private:
    const Type coefficient;
    const int power;

public:
    explicit UTimeTerm(Type a, int n) :
            coefficient(a),
            power(n)
    {

    }

    UTimeTerm(const UTimeTerm& other) :
            coefficient(other.coefficient),
            power(other.power)
    {

    }

    inline Term <Type>* clone() const override {
        return new UTimeTerm(*this);
    }

    inline Type value() const override {
        return coefficient * std::pow(Term<Type>::time, power);
    }

    [[nodiscard]] inline std::vector <Term <Type>*> derivative() const override {
        if (power == 2)
            return {new TimeTerm(power * coefficient)};
        return {new UTimeTerm(power * coefficient, power - 1)};
    }

    [[nodiscard]] bool isPositive() const override {
        return coefficient > 0;
    }

    [[nodiscard]] inline std::string string() const override {
        return (std::stringstream() << coefficient << " × t<sup>" << power <<  "</sup>").str();
    }

    [[nodiscard]] inline std::string unsignedString() const override {
        return (std::stringstream() << std::abs(coefficient) << " × t<sup>" << power <<  "</sup>").str();
    }

    [[nodiscard]] inline Type derivativeConstant() const override {
        return coefficient;
    };
};

template <typename Type>
class ExpUTimeTerm : public Term <Type> {
private:
    const Type coefficient, root;
    const int power;

public:
    explicit ExpUTimeTerm(Type c, Type r, int n) :
            coefficient(c),
            root(r),
            power(n)
    {

    }

    ExpUTimeTerm(const ExpUTimeTerm& other) :
            coefficient(other.coefficient),
            root(other.root),
            power(other.power)
    {

    }

    inline Term <Type>* clone() const override {
        return new ExpUTimeTerm(*this);
    }

    inline Type value() const override {
        return coefficient * std::exp(root * Term<Type>::time) * std::pow(Term<Type>::time, power);
    }

    [[nodiscard]] inline std::vector <Term <Type>*> derivative() const override {
        if (power == 2)
            return {
                    new ExpTimeTerm(2 * coefficient, root),
                    new ExpUTimeTerm(root * coefficient, root, 2)
            };
        return {
                new ExpUTimeTerm(power * coefficient, root, power - 1),
                new ExpUTimeTerm(root * coefficient, root, power)
        };
    }

    [[nodiscard]] bool isPositive() const override {
        return coefficient > 0;
    }

    [[nodiscard]] inline std::string string() const override {
        return (std::stringstream() << coefficient << " × e<sup>" << root << " × t</sup> × t<sup>" << power <<  "</sup>").str();
    }

    [[nodiscard]] inline std::string unsignedString() const override {
        return (std::stringstream() << std::abs(coefficient) << " × e<sup>" << root << " × t</sup> × t<sup>" << power <<  "</sup>").str();
    }
};

template <typename Type>
class CosUTimeTerm : public Term <Type> {
private:
    using Comp = std::complex <Type>;
    const Type amplitude, omega, phi;
    const int power;

public:
    explicit CosUTimeTerm(const Comp& c, const Comp& r, int n) :
            amplitude(2 * std::abs(c)),
            omega(r.imag()),
            phi(std::atan2(c.imag(), c.real())),
            power(n)
    {

    }

    explicit CosUTimeTerm(Type amplitude, Type omega, Type phi, int power) :
            amplitude(amplitude),
            omega(omega),
            phi(std::remainder(phi, 2 * std::numbers::pi_v<Type>)),
            power(power)
    {

    }

    CosUTimeTerm(const CosUTimeTerm& other) :
            amplitude(other.amplitude),
            omega(other.omega),
            phi(other.phi),
            power(other.power)
    {

    }

    inline Term <Type>* clone() const override {
        return new CosUTimeTerm(*this);
    }

    inline Type value() const override {
        return amplitude * std::cos(omega * Term<Type>::time + phi) * std::pow(Term<Type>::time, power);
    }

    [[nodiscard]] inline std::vector <Term <Type>*> derivative() const override {
        if (power == 2)
            return {
                    new CosTimeTerm(2 * amplitude, omega, phi),
                    new CosUTimeTerm(-amplitude * omega, omega, phi - std::numbers::pi_v<Type> / 2, 2)
            };
        return {
                new CosUTimeTerm(power * amplitude, omega, phi, power - 1),
                new CosUTimeTerm(-amplitude * omega, omega, phi - std::numbers::pi_v<Type> / 2, power)
        };
    }

    [[nodiscard]] bool isPositive() const override {
        return amplitude > 0;
    }

    [[nodiscard]] inline std::string string() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × cos(" << omega << " × t) × t<sup>" << power <<  "</sup>").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << ") × t<sup>" << power <<  "</sup>").str();
    }

    [[nodiscard]] inline std::string unsignedString() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × cos(" << omega << " × t) × t<sup>" << power <<  "</sup>").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << ") × t<sup>" << power <<  "</sup>").str();
    }
};

template <typename Type>
class ExpCosUTimeTerm : public Term <Type> {
private:
    using Comp = std::complex <Type>;
    Type amplitude, alpha, omega, phi;
    int power;

public:
    explicit ExpCosUTimeTerm(const Comp& c, const Comp& r, int n) :
            amplitude(2 * std::abs(c)),
            alpha(r.real()),
            omega(r.imag()),
            phi(std::atan2(c.imag(), c.real())),
            power(n)
    {

    }

    explicit ExpCosUTimeTerm(Type amplitude, Type alpha, Type omega, Type phi, int power) :
            amplitude(amplitude),
            alpha(alpha),
            omega(omega),
            phi(std::remainder(phi, 2 * std::numbers::pi_v<Type>)),
            power(power)
    {

    }

    ExpCosUTimeTerm(const ExpCosUTimeTerm& other) :
            amplitude(other.amplitude),
            alpha(other.alpha),
            omega(other.omega),
            phi(other.phi),
            power(other.power)
    {

    }

    inline Term <Type>* clone() const override {
        return new ExpCosUTimeTerm(*this);
    }

    inline Type value() const override {
        return amplitude * std::exp(alpha * Term<Type>::time) * std::cos(omega * Term<Type>::time + phi) * std::pow(Term<Type>::time, power);
    }

    [[nodiscard]] inline std::vector <Term <Type>*> derivative() const override {
        if (power == 2)
            return {
                    new ExpCosTimeTerm(2 * amplitude, alpha, omega, phi),
                    new ExpCosUTimeTerm(alpha * amplitude, alpha, omega, phi, 2),
                    new ExpCosUTimeTerm(-amplitude * omega, alpha, omega, phi - std::numbers::pi_v<Type> / 2, 2)
            };
        return {
                new ExpCosUTimeTerm(power * amplitude, alpha, omega, phi, power - 1),
                new ExpCosUTimeTerm(alpha * amplitude, alpha, omega, phi, power),
                new ExpCosUTimeTerm(-amplitude * omega, alpha, omega, phi - std::numbers::pi_v<Type> / 2, power)
        };
    }

    [[nodiscard]] bool isPositive() const override {
        return amplitude > 0;
    }

    [[nodiscard]] inline std::string string() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t) × t<sup>" << power <<  "</sup>").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << " × t<sup>" << power <<  "</sup>").str();
    }

    [[nodiscard]] inline std::string unsignedString() const override {
        if (phi == 0)
            return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t) × t<sup>" << power <<  "</sup>").str();
        const char phi_sign = phi > 0 ? '+' : '-';
        return (std::stringstream() << amplitude << " × e<sup>" << alpha << " × t</sup> × cos(" << omega << " × t " << phi_sign << ' ' <<  std::abs(phi) << " × t<sup>" << power <<  "</sup>").str();
    }
};

#endif //MATH_CORE_UTIME_TERMS_HPP

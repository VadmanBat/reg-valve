//
// Created by Vadim on 07.01.2025.
//

#ifndef REGVALVE_TERM_EXPRESSION_HPP
#define REGVALVE_TERM_EXPRESSION_HPP

#include "time-terms/utime-terms.hpp"

template <typename Type>
class TermExpression {
private:
    using Comp = std::complex <Type>;
    using VecComp = std::vector <Comp>;

    Type init_value;
    std::vector <Term <Type>*> terms;

public:
    explicit TermExpression() : init_value(0) {

    }

    explicit TermExpression(const VecComp& roots, const VecComp& coeffs, const std::vector <int>& powers) :
            init_value(0)
    {
        static const Type epsilon = std::sqrt(std::numeric_limits<Type>::epsilon());
        const auto n = std::min({roots.size(), coeffs.size(), powers.size()});
        for (std::size_t i = 0; i < n; ++i) {
            if (std::abs(coeffs[i]) < epsilon)
                continue;
            emplace_back(coeffs[i], roots[i], powers[i]);
            if (i + 1 < n && std::abs(roots[i] - std::conj(roots[i + 1])) < epsilon && std::abs(coeffs[i] - std::conj(coeffs[i + 1])) < epsilon)
                ++i;
        }
    }

    explicit TermExpression(const std::vector <Term <Type>*>& terms, Type init_value = 0) :
            init_value(init_value),
            terms(terms)
    {

    }

    explicit TermExpression(std::vector <Term <Type>*>&& terms, Type init_value = 0) :
            init_value(init_value),
            terms(std::move(terms))
    {

    }

    ~TermExpression() {
        for (const auto term : terms)
            delete term;
    }

    void emplace_back(const Comp& c, const Comp& r, int n) {
        if (std::abs(r) == 0)
            switch (n) {
                case 0:
                    init_value += c.real(); return;
                case 1:
                    terms.push_back(new TimeTerm(c.real())); return;
                default:
                    terms.push_back(new UTimeTerm(c.real(), n)); return;
            }

        if (r.imag() == 0)
            switch (n) {
                case 0:
                    terms.push_back(new ExpTerm(c.real(), r.real())); return;
                case 1:
                    terms.push_back(new ExpTimeTerm(c.real(), r.real())); return;
                default:
                    terms.push_back(new ExpUTimeTerm(c.real(), r.real(), n)); return;
            }

        if (r.real() == 0)
            switch (n) {
                case 0:
                    terms.push_back(new CosTerm(c, r)); return;
                case 1:
                    terms.push_back(new CosTimeTerm(c, r)); return;
                default:
                    terms.push_back(new CosUTimeTerm(c, r, n)); return;
            }

        switch (n) {
            case 0:
                terms.push_back(new ExpCosTerm(c, r)); return;
            case 1:
                terms.push_back(new ExpCosTimeTerm(c, r)); return;
            default:
                terms.push_back(new ExpCosUTimeTerm(c, r, n)); return;
        }
    }

    inline void push_back(std::vector <Term <Type>*> new_terms) {
        terms.insert(terms.end(), new_terms.begin(), new_terms.end());
    }

    inline Type operator()(double time) const {
        Term <Type>::time = time;

        Type result = init_value;
        for (const auto term : terms)
            result += term->value();

        return result;
    }

    inline TermExpression derivative() const {
        Type derivative_init_value = 0;
        std::vector <Term <Type>*> derivative_terms;
        for (const auto term : terms) {
            derivative_init_value += term->derivativeConstant();
            auto derivative = term->derivative();
            derivative_terms.insert(
                    derivative_terms.end(),
                    derivative.begin(), derivative.end()
            );
        }
        return TermExpression(derivative_terms, derivative_init_value);
    }

    [[nodiscard]] inline std::string string() const {
        if (init_value != 0) {
            std::string result = (std::stringstream() << init_value).str();
            for (const auto term : terms)
                result += (term->isPositive() ? " + " : " - ") + term->unsignedString();
            return result;
        }
        const auto n = terms.size();
        if (n == 0)
            return {};
        std::string result = terms[0]->string();
        for (std::size_t i = 1; i < n; ++i)
            result += (terms[i]->isPositive() ? " + " : " - ") + terms[i]->unsignedString();
        return result;
    }

    TermExpression& operator=(const TermExpression& other) {
        for (const auto term : terms)
            delete term;
        terms.clear();
        init_value = other.init_value;
        terms.reserve(other.terms.size());
        for (auto term : other.terms)
            terms.push_back(term->clone());
        return *this;
    }

    TermExpression& operator=(TermExpression&& other) noexcept {
        for (const auto term : terms)
            delete term;
        terms.clear();
        init_value = other.init_value;
        terms = std::move(other.terms);
        return *this;
    }
};

#endif //REGVALVE_TERM_EXPRESSION_HPP

//
// Created by Vadma on 08.07.2024.
//

#ifndef REGVALVE_MATCH_CORE_H
#define REGVALVE_MATCH_CORE_H

#include <vector>
#include <complex>
#include <valarray>

class MatchCore {
private:
    using Complex       = std::complex <double>;

    using VectorLD      = std::vector <double>;
    using VectorComp    = std::vector <Complex>;
    using CArray        = std::valarray <Complex>;

public:
    // Функция для вычисления КЧХ по передаточной функции
    static VectorComp calculateFrequencyResponse(
            const VectorComp& numeratorCoeffs,      // Коэффициенты числителя
            const VectorComp& denominatorCoeffs,    // Коэффициенты знаменателя
            const VectorLD& omega                   // Угловая частота
    )
    {
        VectorComp frequencyResponse;
        const std::size_t n(numeratorCoeffs.size());
        const std::size_t m(denominatorCoeffs.size());
        for (const auto& w : omega) {
            Complex s(0, w); // s = jw
            Complex numerator(0, 0);
            Complex denominator(0, 0);
            Complex s_power(1, 0);

            for (std::size_t i = 0; i < n; ++i) { // Вычисляем числитель
                numerator += numeratorCoeffs[i] * s_power;
                s_power *= s; // s^i
            }

            s_power = Complex(1, 0); // Сброс s_power для знаменателя

            for (std::size_t i = 0; i < m; ++i) { // Вычисляем знаменатель
                denominator += denominatorCoeffs[i] * s_power;
                s_power *= s; // s^i
            }

            frequencyResponse.push_back(numerator / denominator);
        }
        return frequencyResponse;
    }
};

#endif //REGVALVE_MATCH_CORE_H

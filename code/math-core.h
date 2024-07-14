//
// Created by Vadma on 08.07.2024.
//

#ifndef REGVALVE_MATH_CORE_H
#define REGVALVE_MATH_CORE_H

#include <vector>
#include <complex>
#include <valarray>

class MathCore {
public:
    using Complex       = std::complex <double>;

    using VectorLD      = std::vector <double>;
    using VectorComp    = std::vector <Complex>;
    using CArray        = std::valarray <Complex>;
    using PairVec       = std::vector <std::pair <double, double>>;

    static VectorLD range(
        const double& first,
        const double& last,
        const double& step
    )
    {
        const int points((last - first) / step);
        VectorLD res(points);
        for (int i = 0; i < points; ++i)
            res[i] = first + i * step;
        return res;
    }

    // Функция для вычисления КЧХ по передаточной функции
    static VectorComp calculateFrequencyResponse(
        const VectorLD& numeratorCoeffs,        // Коэффициенты числителя (обратные)
        const VectorLD& denominatorCoeffs,      // Коэффициенты знаменателя (обратные)
        const VectorLD& frequency               // Угловая частота
    )
    {
        VectorComp frequencyResponse;
        frequencyResponse.reserve(frequency.size());

        for (const auto& w : frequency) {
            Complex s(0, w); // s = jw
            Complex numerator(0);
            Complex denominator(0);

            for (const auto& x : numeratorCoeffs) {
                numerator *= s;
                numerator += x;
            }
            for (const auto& x : denominatorCoeffs) {
                denominator *= s;
                denominator += x;
            }

            frequencyResponse.push_back(numerator / denominator);
        }
        return frequencyResponse;
    }

    static PairVec talbotMethod(
        const VectorLD& numeratorCoeffs,        // Коэффициенты числителя (обратные)
        const VectorLD& denominatorCoeffs,      // Коэффициенты знаменателя (обратные)
        const VectorLD& time                    // Время
    )
    {
        PairVec transientResponse;
        transientResponse.reserve(time.size());

        const int N(100);
        for (const auto& t : time) {
            Complex h_t(0.0);
            for (int k = 1; k <= N; ++k) {
                Complex p(0.0, k * M_PI / t);

                Complex numerator(0);
                Complex denominator(0);
                for (const auto& x : numeratorCoeffs) {
                    numerator *= p;
                    numerator += x;
                }
                for (const auto& x : denominatorCoeffs) {
                    denominator *= p;
                    denominator += x;
                }

                h_t += numerator / denominator * std::exp(p * t);
            }
            transientResponse.emplace_back(t, (2.0 / t) * h_t.real());
        }
        return transientResponse;
    }

    static std::complex <double> H(
        const std::vector<double>& numerator,
        const std::vector<double>& denominator,
        std::complex <double> s)
    {
        std::complex <double> num = 0.0;
        std::complex <double> den = 0.0;

        for (size_t i = 0; i < numerator.size(); ++i) {
            num += numerator[i] * std::pow(s, numerator.size() - 1 - i);
        }

        for (size_t i = 0; i < denominator.size(); ++i) {
            den += denominator[i] * std::pow(s, denominator.size() - 1 - i);
        }

        return num / den;
    }

    static void fft(CArray& x) {
        const size_t N = x.size();
        if (N <= 1) return;

        // Разделение на четные и нечетные элементы
        CArray even = x[std::slice(0, N/2, 2)];
        CArray odd = x[std::slice(1, N/2, 2)];

        // Рекурсивное выполнение FFT
        fft(even);
        fft(odd);

        for (size_t k = 0; k < N/2; ++k) {
            Complex t = std::polar(1.0, -2 * M_PI * k / N) * odd[k];
            x[k] = even[k] + t;
            x[k + N/2] = even[k] - t;
        }
    }

// Функция для вычисления переходной характеристики h(t) с использованием FFT
    static std::vector<std::pair <double, double>> computeImpulseResponseFFT(
        const VectorLD& numerator,
        const VectorLD& denominator,
        const VectorLD& time)
    {
        int n = time.size();
        CArray H_values(n);
        CArray h_values(n);

        // Заполнение значений передаточной функции
        for (int i = 0; i < n; ++i) {
            H_values[i] = H(numerator, denominator, std::complex<double>(time[i], 0));
        }

        // Выполнение обратного FFT
        fft(H_values);
        h_values = H_values.apply(std::conj);
        fft(h_values);
        h_values = h_values.apply(std::conj) / static_cast<double>(n);

        // Извлечение реальной части и нормализация
        std::vector <std::pair <double, double>> h;
        h.reserve(n);
        for (int i = 0; i < n; ++i) {
            h.emplace_back(h_values[i].real() / n, time[i]);
        }

        return h;
    }
};

#endif //REGVALVE_MATH_CORE_H

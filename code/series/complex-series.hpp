//
// Created by Vadim on 01.12.2024.
//

#ifndef REGVALVE_COMPLEX_SERIES_HPP
#define REGVALVE_COMPLEX_SERIES_HPP

#include <cmath>
#include <complex>
#include <vector>

class ComplexSeries {
private:
    using Type = double;
    using Complex = std::complex <Type>;
    using Container = std::vector <Complex>;

    Container data, opt_data;
    Type min_value_x, max_value_x;
    Type min_value_y, max_value_y;

    void optimize() {
        const auto arr = data.data();
        const std::size_t n = data.size();
        for (std::size_t i = n % 2; i < n; i += 2) {
            Type a = arr[i].real();
            Type b = arr[i + 1].real();
            if (a < b) {
                if (a < min_value_x) min_value_x = a;
                if (b > max_value_x) max_value_x = b;
            }
            else {
                if (b < min_value_x) min_value_x = b;
                if (a > max_value_x) max_value_x = a;
            }
            a = arr[i].imag();
            b = arr[i + 1].imag();
            if (a < b) {
                if (a < min_value_y) min_value_y = a;
                if (b > max_value_y) max_value_y = b;
            }
            else {
                if (b < min_value_y) min_value_y = b;
                if (a > max_value_y) max_value_y = a;
            }
        }
        const auto range_x = max_value_x - min_value_x;
        const auto range_y = max_value_y - min_value_y;

        opt_data.reserve(n);
        opt_data.push_back(arr[0]);
        Type x1 = arr[0].real();
        Type y1 = arr[0].imag();
        Type x2, y2, x3, y3;
        for (std::size_t i = 1; i < n - 1; ++i) {
            x2 = arr[i].real();
            y2 = arr[i].imag();
            x3 = arr[i + 1].real();
            y3 = arr[i + 1].imag();
            const auto k = (y3 - y1) / (x3 - x1);
            const auto b = y1 - k * x1;
            const auto dist = std::abs(k * x2 - y2 + b) / std::sqrt(k * k + 1);

            const auto alpha = std::atan(k);
            const auto dist_x = std::sin(alpha) * dist / range_x * 800;
            const auto dist_y = std::cos(alpha) * dist / range_y * 400;

            if (std::sqrt(dist_x * dist_x + dist_y * dist_y) > 1) {
                opt_data.push_back(arr[i]);
                x1 = x2;
                y1 = y2;
            }
        }
        opt_data.push_back(arr[n - 1]);
    }

public:
    ComplexSeries(const Container& values) :
            data(values),
            min_value_x(data[0].real()),
            max_value_x(min_value_x),
            min_value_y(data[0].imag()),
            max_value_y(min_value_y)
    {
        optimize();
    }

    ComplexSeries(Container&& values) :
            data(values),
            min_value_x(data[0].real()),
            max_value_x(min_value_x),
            min_value_y(data[0].imag()),
            max_value_y(min_value_y)
    {
        optimize();
    }

    [[nodiscard]] Container original() const {
        return data;
    }

    [[nodiscard]] Container optimal() const {
        return opt_data;
    }

    [[nodiscard]] Type min_x() const {
        return min_value_x;
    }

    [[nodiscard]] Type max_x() const {
        return max_value_x;
    }

    [[nodiscard]] Type min_y() const {
        return min_value_y;
    }

    [[nodiscard]] Type max_y() const {
        return max_value_y;
    }
};

#endif //REGVALVE_COMPLEX_SERIES_HPP
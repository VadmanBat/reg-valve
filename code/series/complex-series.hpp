#pragma once

#include <cmath>
#include <complex>
#include <vector>

class ComplexSeries {
    using Type      = double;
    using Complex   = std::complex<Type>;
    using Container = std::vector<Complex>;

    Container data, opt_data;
    Type min_value_x{}, max_value_x{};
    Type min_value_y{}, max_value_y{};

    void optimize() {
        if (data.empty())
            return;
        const auto* arr = data.data();
        const std::size_t n = data.size();
        for (std::size_t i = n % 2; i + 1 < n; i += 2) {
            Type a = arr[i].real(), b = arr[i + 1].real();
            if (a < b) {
                if (a < min_value_x)
                    min_value_x = a;
                if (b > max_value_x)
                    max_value_x = b;
            } else {
                if (b < min_value_x)
                    min_value_x = b;
                if (a > max_value_x)
                    max_value_x = a;
            }
            a = arr[i].imag();
            b = arr[i + 1].imag();
            if (a < b) {
                if (a < min_value_y)
                    min_value_y = a;
                if (b > max_value_y)
                    max_value_y = b;
            } else {
                if (b < min_value_y)
                    min_value_y = b;
                if (a > max_value_y)
                    max_value_y = a;
            }
        }
        const auto range_x = max_value_x - min_value_x;
        const auto range_y = max_value_y - min_value_y;
        const Type rx = range_x > 0 ? range_x : 1;
        const Type ry = range_y > 0 ? range_y : 1;

        opt_data.clear();
        opt_data.reserve(n);
        opt_data.push_back(arr[0]);
        Type x1 = arr[0].real(), y1 = arr[0].imag();
        for (std::size_t i = 1; i + 1 < n; ++i) {
            const Type x2 = arr[i].real(), y2 = arr[i].imag();
            const Type x3 = arr[i + 1].real(), y3 = arr[i + 1].imag();
            const auto k = (y3 - y1) / (x3 - x1);
            const auto b = y1 - k * x1;
            const auto dist = std::abs(k * x2 - y2 + b) / std::sqrt(k * k + 1);
            const auto alpha = std::atan(k);
            const auto dist_x = std::sin(alpha) * dist / rx * 800;
            const auto dist_y = std::cos(alpha) * dist / ry * 400;
            if (std::sqrt(dist_x * dist_x + dist_y * dist_y) > 1) {
                opt_data.push_back(arr[i]);
                x1 = x2;
                y1 = y2;
            }
        }
        opt_data.push_back(arr[n - 1]);
    }

public:
    explicit ComplexSeries(Container values)
        : data(std::move(values)),
          min_value_x(data.empty() ? 0 : data.front().real()),
          max_value_x(data.empty() ? 0 : data.front().real()),
          min_value_y(data.empty() ? 0 : data.front().imag()),
          max_value_y(data.empty() ? 0 : data.front().imag()) {
        if (!data.empty())
            optimize();
    }

    [[nodiscard]] const Container& original() const { return data; }
    [[nodiscard]] const Container& optimal() const { return opt_data; }
    [[nodiscard]] Type min_x() const { return min_value_x; }
    [[nodiscard]] Type max_x() const { return max_value_x; }
    [[nodiscard]] Type min_y() const { return min_value_y; }
    [[nodiscard]] Type max_y() const { return max_value_y; }
};

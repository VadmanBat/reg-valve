#include "../transfer-function.hpp"

TransferFunction::VecComp TransferFunction::frequencyResponse() const {
    const Type w_end = binarySearchFrequency(0, 200, 0);
    Type w = 0, dw = w_end / 100, w_next = w + dw, error;
    Complex value_current = frequencyResponse(Complex(0, w));
    Complex value_next_real, value_mid_linear;

    const Type tolerance = 1e-2 * std::max(
            std::abs(frequencyResponse(0)),
            std::abs(frequencyResponse(Complex(0, ternarySearchFrequency(0, w_end))))
    ) / 4;

    VecComp points;
    points.reserve(200);
    points.push_back(frequencyResponse(Complex(0, w)));
    do {
        value_next_real     = frequencyResponse(Complex(0, w_next));
        value_mid_linear    = Complex(0.5, 0) * (value_current + value_next_real);
        error               = std::abs(frequencyResponse(Complex(0, w + 0.5 * dw)) - value_mid_linear);

        if (error < tolerance) {
            points.push_back(value_next_real);
            value_current = value_next_real;
            w = w_next;
            if (error < tolerance / 2)
                dw *= 1.25;
        }
        else
            dw /= 2;
        w_next = w + dw;
    } while (w_next < w_end);
    points.push_back(frequencyResponse(Complex(0, w_end)));
    return points;
}

TransferFunction::VecComp TransferFunction::frequencyResponse(const Pair& range, Type tolerance) const {
    const auto [w_start, w_end] = range;
    Type w = w_start, dw = w_end / 100, w_next = w + dw, error;
    Complex value_current = frequencyResponse(Complex(0, w));
    Complex value_next_real, value_mid_linear;

    tolerance *= std::max(
            std::abs(frequencyResponse(Complex(0, w))),
            std::abs(frequencyResponse(Complex(0, ternarySearchFrequency(0, w_end))))
        ) / 4;

    VecComp points;
    points.reserve(200);
    points.push_back(frequencyResponse(Complex(0, w)));
    do {
        value_next_real     = frequencyResponse(Complex(0, w_next));
        value_mid_linear    = Complex(0.5, 0) * (value_current + value_next_real);
        error               = std::abs(frequencyResponse(Complex(0, w + 0.5 * dw)) - value_mid_linear);

        if (error < tolerance) {
            points.push_back(value_next_real);
            value_current = value_next_real;
            w = w_next;
            if (error < tolerance / 2)
                dw *= 1.25;
        }
        else
            dw /= 2;
        w_next = w + dw;
    } while (w_next < w_end);
    points.push_back(frequencyResponse(Complex(0, w_end)));
    return points;
}

TransferFunction::VecComp TransferFunction::frequencyResponse(const Pair& range, std::size_t points) const {
    VecComp response;
    response.reserve(points);
    for (const auto& frequency : MathCore::logRange(range, points, true))
        response.emplace_back(frequencyResponse(Complex(0, frequency)));
    return response;
}

TransferFunction::VecComp TransferFunction::frequencyResponse(const Pair& range, std::size_t points, bool) const {
    VecComp response;
    response.reserve(points);
    for (const auto& frequency : MathCore::range(range, points))
        response.emplace_back(frequencyResponse(Complex(0, frequency)));
    return response;
}

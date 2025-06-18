#include "../transfer-function.hpp"

TransferFunction::VecPair TransferFunction::impulseResponse() const {
    Type tolerance = 1e-2, t_end;
    if (is_settled) {
        tolerance *= std::abs(impulseFunction(rise_time)) / 2;
        t_end = 2 * settling_time;
    }
    else {
        Type max = std::abs(impulseFunction(200));
        for (int time = 190; time < 200; ++time)
            max = std::max(max, std::abs(impulseFunction(time)));
        tolerance *= max;
        t_end = 200;
    }

    Type t = 0, y_current = impulseFunction(t);
    Type dt = t_end / 100;
    Type t_next = t + dt, y_next_real, y_mid_linear, error;

    VecPair points;
    points.reserve(200);
    points.emplace_back(t, impulseFunction(t));
    do {
        y_next_real     = impulseFunction(t_next);
        y_mid_linear    = (y_current + y_next_real) / 2;
        error           = std::abs(impulseFunction(t + 0.5 * dt) - y_mid_linear);

        if (error < tolerance) {
            points.emplace_back(t_next, y_next_real);
            y_current = y_next_real;
            t = t_next;
            if (error < tolerance / 2)
                dt *= 1.25;
        }
        else
            dt /= 2;
        t_next = t + dt;
    } while (t_next < t_end);
    points.emplace_back(t_end, impulseFunction(t_end));
    return points;
}

TransferFunction::VecPair TransferFunction::impulseResponse(const Pair& range, Type tolerance) const {
    tolerance *= std::abs(impulseFunction(rise_time)) / 2;
    const auto [t_start, t_end] = range;
    double t = t_start, y_current = impulseFunction(t);
    double dt = t_end / 100;
    double t_next = t + dt, y_next_real, y_mid_linear, error;

    VecPair points;
    points.reserve(200);
    points.emplace_back(t, impulseFunction(t));
    do {
        y_next_real     = impulseFunction(t_next);
        y_mid_linear    = (y_current + y_next_real) / 2;
        error           = std::abs(impulseFunction(t + 0.5 * dt) - y_mid_linear);

        if (error < tolerance) {
            points.emplace_back(t_next, y_next_real);
            y_current = y_next_real;
            t = t_next;
            if (error < tolerance / 2)
                dt *= 1.25;
        }
        else
            dt /= 2;
        t_next = t + dt;
    } while (t_next < t_end);
    points.emplace_back(t_end, impulseFunction(t_end));
    return points;
}

TransferFunction::VecPair TransferFunction::impulseResponse(const Pair& range, std::size_t points) const {
    VecPair response;
    response.reserve(points);
    for (const auto& time : MathCore::range(range, points))
        response.emplace_back(time, impulseFunction(time));
    return response;
}
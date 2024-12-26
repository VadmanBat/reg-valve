#include "../transfer-function.hpp"

TransferFunction::VecPair TransferFunction::transientResponse() const {
    Type tolerance = 1e-2, t_end;
    if (is_settled) {
        tolerance *= std::abs(steady_state_value);
        t_end = 2 * settling_time;
    }
    else {
        Type max = std::abs(transientResponse(190));
        for (int time = 190; time < 200; ++time)
            max = std::max(max, std::abs(transientResponse(time)));
        tolerance *= 2 * max;
        t_end = 200;
    }

    Type t = 0, y_current = transientResponse(t);
    Type dt = std::min(tolerance / (std::abs(impulseResponse(t)) + 1e-9), t_end / 100);
    Type t_next = t + dt, y_next_real, y_next_linear, error;

    VecPair points;
    points.reserve(200);
    points.emplace_back(t, transientResponse(t));
    do {
        y_next_real     = transientResponse(t_next);
        y_next_linear   = y_current + dt * impulseResponse(t);
        error           = std::abs(y_next_real - y_next_linear);

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
    points.emplace_back(t_end, transientResponse(t_end));
    return points;
}

TransferFunction::VecPair TransferFunction::transientResponse(const Pair& range, Type tolerance) const {
    const auto [t_start, t_end] = range;
    if (is_settled) {
        tolerance *= std::abs(steady_state_value);
    }
    else {
        Type max = std::abs(transientResponse(190));
        for (int time = 190; time < 200; ++time)
            max = std::max(max, std::abs(transientResponse(time)));
        tolerance *= 2 * max;
    }

    Type t = t_start, y_current = transientResponse(t);
    Type dt = std::min(tolerance / (std::abs(impulseResponse(t)) + 1e-9), t_end / 100);
    Type t_next = t + dt, y_next_real, y_next_linear, error;

    VecPair points;
    points.reserve(200);
    points.emplace_back(t, transientResponse(t));
    do {
        y_next_real     = transientResponse(t_next);
        y_next_linear   = y_current + dt * impulseResponse(t);
        error           = std::abs(y_next_real - y_next_linear);

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
    points.emplace_back(t_end, transientResponse(t_end));
    return points;
}

TransferFunction::VecPair TransferFunction::transientResponse(const Pair& range, std::size_t points) const {
    VecPair response;
    response.reserve(points);
    for (const auto& time : MathCore::range(range, points))
        response.emplace_back(time, transientResponse(time));
    return response;
}
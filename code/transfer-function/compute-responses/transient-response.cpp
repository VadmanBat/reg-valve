#include "../transfer-function.hpp"

TransferFunction::VecPair TransferFunction::transientResponse() const {
    Type tolerance = 1e-2, t_end;
    if (is_settled) {
        //tolerance *= std::abs(transientResponse(peak_time));
        tolerance *= std::abs(numerator.back() != 0 ? steady_state_value : transientFunction(0));
        t_end = 2 * settling_time;
    }
    else {
        Type max = std::abs(transientFunction(200));
        for (int time = 190; time < 200; ++time)
            max = std::max(max, std::abs(transientFunction(time)));
        tolerance *= 2 * max;
        t_end = 200;
    }

    Type t = 0, y_current = transientFunction(t);
    Type dt = std::min(tolerance / (std::abs(impulseFunction(t)) + 1e-9), t_end / 100);
    Type t_next = t + dt, y_next_real, y_next_linear, error;

    VecPair points;
    points.reserve(200);
    points.emplace_back(t, transientFunction(t));
    do {//std::cout << "compute: " << t << '\n';
        y_next_real     = transientFunction(t_next);
        y_next_linear   = y_current + dt * impulseFunction(t);
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
    //std::cout << "compute is finished\n";
    points.emplace_back(t_end, transientFunction(t_end));
    return points;
}

TransferFunction::VecPair TransferFunction::transientResponse(const Pair& range, Type tolerance) const {
    const auto [t_start, t_end] = range;
    if (is_settled) {
        tolerance *= std::abs(numerator.back() != 0 ? steady_state_value : transientFunction(0));
    }
    else {
        const Type step = 0.005 * (t_end - t_start), time = t_end - 10 * step;
        Type max = std::abs(transientFunction(t_end));
        for (int i = 0; i < 10; ++i)
            max = std::max(max, std::abs(transientFunction(time + i * step)));
        tolerance *= max;
    }

    Type t = t_start, y_current = transientFunction(t);
    Type dt = std::min(tolerance / (std::abs(impulseFunction(t)) + 1e-9), t_end / 100);
    Type t_next = t + dt, y_next_real, y_next_linear, error;

    VecPair points;
    points.reserve(200);
    points.emplace_back(t, transientFunction(t));
    do {
        y_next_real     = transientFunction(t_next);
        y_next_linear   = y_current + dt * impulseFunction(t);
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
    points.emplace_back(t_end, transientFunction(t_end));
    return points;
}

TransferFunction::VecPair TransferFunction::transientResponse(const Pair& range, std::size_t points) const {
    VecPair response;
    response.reserve(points);
    for (const auto& time : MathCore::range(range, points))
        response.emplace_back(time, transientFunction(time));
    return response;
}
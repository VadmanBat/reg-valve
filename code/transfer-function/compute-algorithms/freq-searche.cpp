#include "../transfer-function.hpp"

TransferFunction::Type TransferFunction::binarySearchFrequency(Type min, Type max, Type value, Type tolerance) const {
    tolerance *= max - min;
    Type mid = (min + max) / 2;
    while (max - min > tolerance) {
        (std::abs(frequencyResponse(mid)) > value ? min : max) = mid;
        mid = (min + max) / 2;
    }
    return mid;
} /// log(N)

#define value(X) std::abs(frequencyResponse(Complex(0, X)))
TransferFunction::Type TransferFunction::ternarySearchFrequency(Type min, Type max, Type tolerance) const {
    tolerance *= max - min;
    double step, new_min, new_max;
    while (max - min > tolerance) {
        step = (max - min) / 3;
        new_min = min + step;
        new_max = max - step;
        if (value(new_min) < value(new_max))
            min = new_min;
        else
            max = new_max;
    }
    return (min + max) / 2;
} /// log(N)
#undef value

TransferFunction::Pair TransferFunction::computeFrequencyRange(Type min, Type max) const {
    return {
            binarySearchFrequency(min, max, std::abs(frequencyResponse(0))),
            binarySearchFrequency(min, max, 0)
    };
} /// log(N)
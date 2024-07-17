//
// Created by Vadma on 08.07.2024.
//

#ifndef REGVALVE_CONVERT_CORE_HPP
#define REGVALVE_CONVERT_CORE_HPP

#include <vector>
#include <complex>

#include <QVector>
#include <QPointF>

namespace ProjectTypes {
    using Type      = double;
    using Complex   = std::complex <Type>;
}

class ConvertCore {
public:
    using Type      = ProjectTypes::Type;
    using Complex   = ProjectTypes::Complex;

    using Vec       = std::vector <Type>;
    using VecPair   = std::vector <std::pair <Type, Type>>;
    using VecComp   = std::vector <Complex>;
    using ValComp   = std::valarray <Complex>;

    static QVector <QPointF> convertToQVectorPointF(const std::vector <std::complex<double>>& complexVector) {
        QVector <QPointF> points;
        for (const auto& complexNumber : complexVector)
            points.push_back(QPointF(complexNumber.real(), complexNumber.imag()));
        return points;
    }

    template <typename Type>
    static std::vector <Complex> toComplexVector(const std::vector <Type>& numbers) {
        std::vector <Complex> res;
        res.reserve(numbers.size());
        for (const auto& number : numbers)
            res.push_back(number);
        return res;
    } /// N

    static Complex normalize(const Complex& z, const Type& epsilon = 1e-9) {
        Type real = (std::abs(z.real()) < epsilon) ? 0 : z.real();
        Type imag = (std::abs(z.imag()) < epsilon) ? 0 : z.imag();
        return {real, imag};
    } /// 1
};

#endif //REGVALVE_CONVERT_CORE_HPP

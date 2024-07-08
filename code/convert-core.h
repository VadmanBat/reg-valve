//
// Created by Vadma on 08.07.2024.
//

#ifndef REGVALVE_CONVERT_CORE_H
#define REGVALVE_CONVERT_CORE_H

#include <vector>
#include <complex>
#include <QVector>
#include <QPointF>

class ConvertCore {
public:
    static QVector <QPointF> convertToQVectorPointF(const std::vector <std::complex<double>>& complexVector) {
        QVector <QPointF> points;
        for (const auto& complexNumber : complexVector)
            points.push_back(QPointF(complexNumber.real(), complexNumber.imag()));
        return points;
    }
};

#endif //REGVALVE_CONVERT_CORE_H

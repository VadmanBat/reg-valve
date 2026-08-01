#pragma once

#include "code/charts/utils/chart-utils.hpp"

#include <QChart>
#include <QLineSeries>
#include <QList>
#include <QPointF>
#include <QValueAxis>

namespace chart_utils {
namespace detail {

[[nodiscard]] inline bool isGuideSeries(const QString& name) {
    return name == QLatin1String(kHorGuide) || name == QLatin1String(kVerGuide);
}

[[nodiscard]] inline QList<QPointF> toPoints(const VecPair& data) {
    QList<QPointF> pts;
    pts.reserve(static_cast<int>(data.size()));
    for (const auto& [x, y] : data)
        pts.append(QPointF(x, y));
    return pts;
}

[[nodiscard]] inline QList<QPointF> toPoints(const VecComp& data) {
    QList<QPointF> pts;
    pts.reserve(static_cast<int>(data.size()));
    for (const auto& z : data)
        pts.append(QPointF(z.real(), z.imag()));
    return pts;
}

[[nodiscard]] inline QLineSeries* lastDataSeries(QChart* chart) {
    if (!chart)
        return nullptr;
    const auto all = chart->series();
    for (auto i = all.size(); i > 0; --i) {
        auto* s = qobject_cast<QLineSeries*>(all[i - 1]);
        if (!s || isGuideSeries(s->name()))
            continue;
        return s;
    }
    return nullptr;
}

inline void attachToAxes(QChart* chart, QAbstractSeries* series) {
    auto* axis_x = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0, nullptr));
    auto* axis_y = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).value(0, nullptr));
    if (axis_x && !series->attachedAxes().contains(axis_x))
        series->attachAxis(axis_x);
    if (axis_y && !series->attachedAxes().contains(axis_y))
        series->attachAxis(axis_y);
}

} // namespace detail
} // namespace chart_utils

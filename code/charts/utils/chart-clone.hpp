#pragma once

#include "code/charts/utils/chart-utils.hpp"

#include <QBrush>
#include <QChart>
#include <QLegend>
#include <QLineSeries>
#include <QValueAxis>

namespace chart_utils {

/// Deep-copy series, titles and ranges into a new QChart (viewer window).
[[nodiscard]] inline QChart* cloneChart(QChart* src) {
    auto* dst = new QChart;
    dst->setTitle(src->title());
    dst->setAnimationOptions(QChart::NoAnimation);
    dst->setBackgroundVisible(true);
    dst->setBackgroundBrush(QBrush(Qt::white));
    dst->setPlotAreaBackgroundVisible(true);
    dst->setPlotAreaBackgroundBrush(QBrush(Qt::white));
    if (src->legend())
        dst->legend()->setVisible(src->legend()->isVisible());

    QString title_x;
    QString title_y;
    Pair range_x{-1.0, 1.0};
    Pair range_y{-1.0, 1.0};
    if (auto* ax = qobject_cast<QValueAxis*>(src->axes(Qt::Horizontal).value(0, nullptr))) {
        title_x        = ax->titleText();
        range_x.first  = ax->min();
        range_x.second = ax->max();
    }
    if (auto* ay = qobject_cast<QValueAxis*>(src->axes(Qt::Vertical).value(0, nullptr))) {
        title_y        = ay->titleText();
        range_y.first  = ay->min();
        range_y.second = ay->max();
    }

    createAxes(dst, title_x, title_y);

    for (auto* s : src->series()) {
        auto* line = qobject_cast<QLineSeries*>(s);
        if (!line)
            continue;
        auto* copy = new QLineSeries;
        copy->setName(line->name());
        copy->setPen(line->pen());
        copy->setPointsVisible(line->pointsVisible());
        copy->replace(line->points());
        dst->addSeries(copy);
    }

    updateAxes(dst, range_x, range_y);
    return dst;
}

} // namespace chart_utils

#include "code/charts/utils/chart-utils.hpp"

#include "code/charts/utils/chart-utils-detail.hpp"

#include <QBrush>
#include <QLegend>
#include <QLineSeries>
#include <QPainter>
#include <QValueAxis>

namespace chart_utils {

QPen penForIndex(std::size_t index) {
    static const QPen pens[6] = {
        QPen(QColor(0x1f, 0x77, 0xb4), 2), QPen(QColor(0xff, 0x7f, 0x0e), 2), QPen(QColor(0x2c, 0xa0, 0x2c), 2),
        QPen(QColor(0xd6, 0x27, 0x28), 2), QPen(QColor(0x94, 0x67, 0xbd), 2), QPen(QColor(0x8c, 0x56, 0x4b), 2),
    };
    return pens[index % 6];
}

QChartView* makeChartView(QChart* chart, QWidget* parent, const QString& title, const QString& titleX,
                          const QString& titleY) {
    chart->setTitle(title);
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->setBackgroundVisible(true);
    chart->setBackgroundBrush(QBrush(Qt::white));
    chart->setPlotAreaBackgroundVisible(true);
    chart->setPlotAreaBackgroundBrush(QBrush(Qt::white));
    chart->legend()->setVisible(true);
    createAxes(chart, titleX, titleY);

    updateAxes(chart, {-1.0, 1.0}, {-1.0, 1.0}, GridMode::Tab, /*snap_x=*/false, /*snap_y=*/false);

    auto* view = new QChartView(chart, parent);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    view->setBackgroundBrush(QBrush(Qt::white));
    createChartContextMenu(view);
    return view;
}

QChart* cloneChart(QChart* src) {
    if (!src)
        return new QChart;

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
        if (!line || detail::isGuideSeries(line->name()))
            continue;
        auto* copy = new QLineSeries;
        copy->setName(line->name());
        copy->setPen(line->pen());
        copy->setPointsVisible(line->pointsVisible());
        copy->replace(line->points());
        dst->addSeries(copy);
        detail::attachToAxes(dst, copy);
    }

    // Preserve source extents; viewer switches to GridMode::Viewer after open.
    updateAxes(dst, range_x, range_y, GridMode::Tab, /*snap_x=*/false, /*snap_y=*/false);
    return dst;
}

} // namespace chart_utils

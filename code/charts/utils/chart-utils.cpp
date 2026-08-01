#include "code/charts/utils/chart-utils.hpp"
#include "code/charts/utils/chart-utils-detail.hpp"

#include <QBrush>
#include <QLegend>
#include <QLegendMarker>
#include <QLineSeries>
#include <QPainter>
#include <QValueAxis>

namespace chart_utils {
namespace {

QLineSeries* find_series_by_name(QChart* chart, const char* name) {
    if (!chart)
        return nullptr;
    for (auto* s : chart->series()) {
        if (s->name() == QLatin1String(name))
            return qobject_cast<QLineSeries*>(s);
    }
    return nullptr;
}

void hide_legend_marker(QChart* chart, QAbstractSeries* series) {
    if (!chart || !series || !chart->legend())
        return;
    for (auto* marker : chart->legend()->markers(series))
        marker->setVisible(false);
}

QPen guide_pen() {
    QPen pen(QColor(0x12, 0x12, 0x12), 2.0);
    pen.setCosmetic(true);
    pen.setCapStyle(Qt::FlatCap);
    pen.setStyle(Qt::SolidLine);
    return pen;
}

} // namespace

Pair computeAxesRange(double min, double max) {
    return niceAxisRange(min, max, /*include_zero=*/true);
}

QPen penForIndex(std::size_t index) {
    static const QPen pens[6] = {
        QPen(QColor(0x1f, 0x77, 0xb4), 2), QPen(QColor(0xff, 0x7f, 0x0e), 2), QPen(QColor(0x2c, 0xa0, 0x2c), 2),
        QPen(QColor(0xd6, 0x27, 0x28), 2), QPen(QColor(0x94, 0x67, 0xbd), 2), QPen(QColor(0x8c, 0x56, 0x4b), 2),
    };
    return pens[index % 6];
}

void createAxes(QChart* chart, const QString& titleX, const QString& titleY) {
    auto* axis_x = new QValueAxis(chart);
    auto* axis_y = new QValueAxis(chart);
    axis_x->setTitleText(titleX);
    axis_y->setTitleText(titleY);
    chart->addAxis(axis_x, Qt::AlignBottom);
    chart->addAxis(axis_y, Qt::AlignLeft);
}

void updateOriginGuides(QChart* chart, const Pair& range_x, const Pair& range_y) {
    if (!chart)
        return;

    auto ensure = [chart](const char* name) -> QLineSeries* {
        auto* s = find_series_by_name(chart, name);
        if (s)
            return s;
        s = new QLineSeries;
        s->setName(QString::fromLatin1(name));
        s->setPen(guide_pen());
        s->setPointsVisible(false);
        chart->addSeries(s);
        detail::attachToAxes(chart, s);
        hide_legend_marker(chart, s);
        return s;
    };

    auto* hor = ensure(kHorGuide);
    auto* ver = ensure(kVerGuide);

    hor->replace(QList<QPointF>{{range_x.first, 0.0}, {range_x.second, 0.0}});
    ver->replace(QList<QPointF>{{0.0, range_y.first}, {0.0, range_y.second}});

    detail::attachToAxes(chart, hor);
    detail::attachToAxes(chart, ver);
    hide_legend_marker(chart, hor);
    hide_legend_marker(chart, ver);
}

void removeAllSeries(QChart* chart) {
    const auto all = chart->series();
    for (auto* series : all) {
        if (detail::isGuideSeries(series->name()))
            continue;
        chart->removeSeries(series);
        delete series;
    }
    chart->update();
}

void updateAxes(QChart* chart, const Pair& range_x, const Pair& range_y) {
    auto* axis_x = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0, nullptr));
    auto* axis_y = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).value(0, nullptr));
    if (!axis_x || !axis_y)
        return;
    axis_x->setRange(range_x.first, range_x.second);
    axis_y->setRange(range_y.first, range_y.second);

    updateOriginGuides(chart, range_x, range_y);

    for (auto* series : chart->series()) {
        if (!series->attachedAxes().contains(axis_x))
            series->attachAxis(axis_x);
        if (!series->attachedAxes().contains(axis_y))
            series->attachAxis(axis_y);
    }
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

    const Pair def_x{-1.0, 1.0};
    const Pair def_y{-1.0, 1.0};
    if (auto* axis_x = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0, nullptr)))
        axis_x->setRange(def_x.first, def_x.second);
    if (auto* axis_y = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).value(0, nullptr)))
        axis_y->setRange(def_y.first, def_y.second);
    updateOriginGuides(chart, def_x, def_y);

    auto* view = new QChartView(chart, parent);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setBackgroundBrush(QBrush(Qt::white));
    createChartContextMenu(view);
    return view;
}

} // namespace chart_utils

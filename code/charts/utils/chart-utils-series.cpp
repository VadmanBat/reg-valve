#include "code/charts/utils/chart-utils-detail.hpp"
#include "code/charts/utils/chart-utils.hpp"

#include <QFile>
#include <QLineSeries>
#include <QTextStream>
#include <QXYSeries>

namespace chart_utils {

void addRealSeries(QChart* chart, const VecPair& points, const QString& title, std::size_t index) {
    if (points.empty())
        return;
    auto* series = new QLineSeries;
    series->setName(title);
    series->setPen(penForIndex(index));
    series->replace(detail::toPoints(points));
    chart->addSeries(series);
}

void addComplexSeries(QChart* chart, const VecComp& points, const QString& title, std::size_t index) {
    if (points.empty())
        return;
    auto* series = new QLineSeries;
    series->setName(title);
    series->setPen(penForIndex(index));
    series->replace(detail::toPoints(points));
    chart->addSeries(series);
}

bool replaceLastRealSeries(QChart* chart, const VecPair& points, const QString& title) {
    auto* series = detail::lastDataSeries(chart);
    if (!series || points.empty())
        return false;
    series->setName(title);
    series->replace(detail::toPoints(points));
    return true;
}

bool replaceLastComplexSeries(QChart* chart, const VecComp& points, const QString& title) {
    auto* series = detail::lastDataSeries(chart);
    if (!series || points.empty())
        return false;
    series->setName(title);
    series->replace(detail::toPoints(points));
    return true;
}

bool saveChartToFile(const QString& fileName, QChart* chart) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream out(&file);
    for (QAbstractSeries* series : chart->series()) {
        if (detail::isGuideSeries(series->name()))
            continue;
        out << "Name: " << series->name() << '\n';
        if (auto* xy = qobject_cast<QXYSeries*>(series)) {
            const int n = xy->count();
            for (int i = 0; i < n; ++i)
                out << xy->at(i).x() << ", " << xy->at(i).y() << '\n';
        }
        out << '\n';
    }
    return true;
}

} // namespace chart_utils

#include "code/charts/chart_panel.h"

#include <QChart>
#include <QChartView>
#include <QVBoxLayout>
#include <QValueAxis>

ChartPanel::ChartPanel(const QString& title, const QString& titleX, const QString& titleY, QWidget* parent)
    : QWidget(parent) {
    setObjectName(QStringLiteral("ChartPanel"));
    chart_ = new QChart;
    view_  = chart_utils::makeChartView(chart_, this, title, titleX, titleY);
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->addWidget(view_);
}

void ChartPanel::setChartTitle(const QString& title) {
    chart_->setTitle(title);
}

void ChartPanel::setAxisTitles(const QString& titleX, const QString& titleY) {
    if (auto* axisX = qobject_cast<QValueAxis*>(chart_->axes(Qt::Horizontal).value(0, nullptr)))
        axisX->setTitleText(titleX);
    if (auto* axisY = qobject_cast<QValueAxis*>(chart_->axes(Qt::Vertical).value(0, nullptr)))
        axisY->setTitleText(titleY);
}

void ChartPanel::addRealCurve(const VecPair& points, const QString& name, bool useOptimizedSubset) {
    chart_utils::addRealSeries(chart_, points, name, curveCount_, useOptimizedSubset);
    ++curveCount_;
}

void ChartPanel::addComplexCurve(const VecComp& points, const QString& name, bool useOptimizedSubset) {
    chart_utils::addComplexSeries(chart_, points, name, curveCount_, useOptimizedSubset);
    ++curveCount_;
}

bool ChartPanel::replaceLastRealCurve(const VecPair& points, const QString& name, bool useOptimizedSubset) {
    if (curveCount_ == 0)
        return false;
    if (chart_utils::replaceLastRealSeries(chart_, points, name, useOptimizedSubset))
        return true;
    // Fallback: re-add with previous pen index
    chart_utils::addRealSeries(chart_, points, name, curveCount_ - 1, useOptimizedSubset);
    return true;
}

bool ChartPanel::replaceLastComplexCurve(const VecComp& points, const QString& name, bool useOptimizedSubset) {
    if (curveCount_ == 0)
        return false;
    if (chart_utils::replaceLastComplexSeries(chart_, points, name, useOptimizedSubset))
        return true;
    chart_utils::addComplexSeries(chart_, points, name, curveCount_ - 1, useOptimizedSubset);
    return true;
}

void ChartPanel::removeLastCurve() {
    if (curveCount_ == 0)
        return;
    chart_utils::eraseLastSeries(chart_);
    --curveCount_;
}

void ChartPanel::clearCurves() {
    chart_utils::removeAllSeries(chart_);
    curveCount_ = 0;
}

void ChartPanel::fitAxes(double minX, double maxX, double minY, double maxY, bool padY) {
    const auto rx = chart_utils::Pair{minX, maxX};
    const auto ry = padY ? chart_utils::computeAxesRange(minY, maxY) : chart_utils::Pair{minY, maxY};
    chart_utils::updateAxes(chart_, rx, ry);
}

void ChartPanel::fitAxes(const Pair& rangeX, const Pair& rangeY, bool padY) {
    fitAxes(rangeX.first, rangeX.second, rangeY.first, rangeY.second, padY);
}

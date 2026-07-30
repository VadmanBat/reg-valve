#include "code/charts/chart-panel.h"

#include <QChart>
#include <QChartView>
#include <QVBoxLayout>

ChartPanel::ChartPanel(const QString& title, const QString& titleX, const QString& titleY, QWidget* parent)
    : QWidget(parent) {
    setObjectName(QStringLiteral("ChartPanel"));
    chart_  = new QChart;
    view_   = chart_utils::makeChartView(chart_, this, title, titleX, titleY);
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->addWidget(view_);
}

void ChartPanel::addRealCurve(const VecPair& points, const QString& name) {
    chart_utils::addRealSeries(chart_, points, name, curve_count_);
    ++curve_count_;
}

void ChartPanel::addComplexCurve(const VecComp& points, const QString& name) {
    chart_utils::addComplexSeries(chart_, points, name, curve_count_);
    ++curve_count_;
}

bool ChartPanel::replaceLastRealCurve(const VecPair& points, const QString& name) {
    if (curve_count_ == 0)
        return false;
    if (chart_utils::replaceLastRealSeries(chart_, points, name))
        return true;
    chart_utils::addRealSeries(chart_, points, name, curve_count_ - 1);
    return true;
}

bool ChartPanel::replaceLastComplexCurve(const VecComp& points, const QString& name) {
    if (curve_count_ == 0)
        return false;
    if (chart_utils::replaceLastComplexSeries(chart_, points, name))
        return true;
    chart_utils::addComplexSeries(chart_, points, name, curve_count_ - 1);
    return true;
}

void ChartPanel::clearCurves() {
    chart_utils::removeAllSeries(chart_);
    curve_count_ = 0;
    // Keep origin cross and a neutral frame
    constexpr chart_utils::Pair def{-1.0, 1.0};
    chart_utils::updateAxes(chart_, def, def);
}

void ChartPanel::fitAxes(double minX, double maxX, double minY, double maxY, bool /*padY*/) {
    // Nice outward bounds (1–2–5×10^k) + always include origin for axis cross.
    const auto rx = chart_utils::niceAxisRange(minX, maxX, /*include_zero=*/true);
    const auto ry = chart_utils::niceAxisRange(minY, maxY, /*include_zero=*/true);
    chart_utils::updateAxes(chart_, rx, ry);
}

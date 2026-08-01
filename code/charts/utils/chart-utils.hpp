#pragma once

#include "code/charts/utils/nice-axis.hpp"

#include <complex>
#include <QChart>
#include <QChartView>
#include <QPen>
#include <QString>
#include <utility>
#include <vector>

namespace chart_utils {

using Pair    = std::pair<double, double>;
using VecPair = std::vector<Pair>;
using VecComp = std::vector<std::complex<double>>;

/// Guide series names (bold black axes through origin). Skipped by save / dialog / clear.
inline constexpr auto kHorGuide = "hor-line";
inline constexpr auto kVerGuide = "ver-line";

QPen penForIndex(std::size_t index);

void createAxes(QChart* chart, const QString& titleX, const QString& titleY);
void createChartContextMenu(QChartView* chartView);
/// Open detached interactive viewer (non-modal). Safe if chart is null.
void openChartViewer(QChart* chart, QWidget* parent = nullptr);
void removeAllSeries(QChart* chart);

/// Legacy alias → niceAxisRange(..., include_zero=true).
[[nodiscard]] Pair computeAxesRange(double min, double max);

void updateAxes(QChart* chart, const Pair& range_x, const Pair& range_y);

/// Ensure bold black cross at (0,0) spanning current ranges.
void updateOriginGuides(QChart* chart, const Pair& range_x, const Pair& range_y);

void addRealSeries(QChart* chart, const VecPair& points, const QString& title, std::size_t index = 0);
void addComplexSeries(QChart* chart, const VecComp& points, const QString& title, std::size_t index = 0);

bool replaceLastRealSeries(QChart* chart, const VecPair& points, const QString& title);
bool replaceLastComplexSeries(QChart* chart, const VecComp& points, const QString& title);

bool saveChartToFile(const QString& fileName, QChart* chart);

QChartView* makeChartView(QChart* chart, QWidget* parent, const QString& title, const QString& titleX,
                          const QString& titleY);

} // namespace chart_utils

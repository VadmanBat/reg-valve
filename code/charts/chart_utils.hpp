#pragma once

#include <QChart>
#include <QChartView>
#include <QPen>
#include <QString>

#include <complex>
#include <utility>
#include <vector>

namespace chart_utils {

using Pair    = std::pair<double, double>;
using VecPair = std::vector<Pair>;
using VecComp = std::vector<std::complex<double>>;

QPen penForIndex(std::size_t index);

void createAxes(QChart* chart, const QString& titleX, const QString& titleY);
void createChartContextMenu(QChartView* chartView);
void eraseLastSeries(QChart* chart);
void removeAllSeries(QChart* chart);
Pair computeAxesRange(double min, double max);
void updateAxes(QChart* chart, const Pair& range_x, const Pair& range_y);

/// Fast path: single replace of points (preferred over per-point append).
void addRealSeries(QChart* chart, const VecPair& points, const QString& title, std::size_t index = 0,
                   bool useOptimizedSubset = true);
void addComplexSeries(QChart* chart, const VecComp& points, const QString& title, std::size_t index = 0,
                      bool useOptimizedSubset = true);

/// Update last data series in place (no remove/add) — for high-FPS slider refresh.
bool replaceLastRealSeries(QChart* chart, const VecPair& points, const QString& title,
                           bool useOptimizedSubset = true);
bool replaceLastComplexSeries(QChart* chart, const VecComp& points, const QString& title,
                              bool useOptimizedSubset = true);

bool saveChartToFile(const QString& fileName, QChart* chart);
VecPair readVectorFromFile(const QString& fileName);

QChartView* makeChartView(QChart* chart, QWidget* parent, const QString& title, const QString& titleX,
                          const QString& titleY);

/// Primary screen refresh rate (Hz). Falls back to 60 if unknown.
[[nodiscard]] double displayRefreshHz();
/// One frame duration in ms for QTimer (min 1). Matches display refresh when available.
[[nodiscard]] int frameIntervalMs();

} // namespace chart_utils

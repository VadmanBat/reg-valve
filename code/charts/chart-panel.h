#pragma once

#include "code/charts/chart-utils.hpp"

#include <QWidget>

#include <complex>
#include <utility>
#include <vector>

class QChart;
class QChartView;
class QVBoxLayout;

/// Self-contained chart widget: owns QChart + QChartView, context menu, series API.
class ChartPanel : public QWidget {
    Q_OBJECT
public:
    using Pair    = chart_utils::Pair;
    using VecPair = chart_utils::VecPair;
    using VecComp = chart_utils::VecComp;

    explicit ChartPanel(const QString& title, const QString& titleX, const QString& titleY,
                        QWidget* parent = nullptr);

    [[nodiscard]] QChart* chart() const { return chart_; }
    [[nodiscard]] QChartView* view() const { return view_; }
    [[nodiscard]] std::size_t curveCount() const { return curveCount_; }

    void setChartTitle(const QString& title);
    void setAxisTitles(const QString& titleX, const QString& titleY);

    void addRealCurve(const VecPair& points, const QString& name, bool useOptimizedSubset = true);
    void addComplexCurve(const VecComp& points, const QString& name, bool useOptimizedSubset = true);

    /// In-place update of the last data curve (slider / replace).
    bool replaceLastRealCurve(const VecPair& points, const QString& name, bool useOptimizedSubset = true);
    bool replaceLastComplexCurve(const VecComp& points, const QString& name, bool useOptimizedSubset = true);

    void removeLastCurve();
    void clearCurves();

    /// Set axis ranges; pads Y via chart_utils::computeAxesRange when padY is true.
    void fitAxes(double minX, double maxX, double minY, double maxY, bool padY = true);
    void fitAxes(const Pair& rangeX, const Pair& rangeY, bool padY = true);

private:
    QChart* chart_{nullptr};
    QChartView* view_{nullptr};
    QVBoxLayout* layout_{nullptr};
    std::size_t curveCount_{0};
};

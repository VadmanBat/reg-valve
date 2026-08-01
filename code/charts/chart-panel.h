#pragma once

#include "code/charts/utils/chart-utils.hpp"

#include <QWidget>

class QChart;
class QChartView;
class QVBoxLayout;

/// Self-contained chart widget: owns QChart + QChartView, context menu, series API.
class ChartPanel : public QWidget {
    Q_OBJECT

private:
    QChart* chart_{nullptr};
    QChartView* view_{nullptr};
    QVBoxLayout* layout_{nullptr};
    std::size_t curve_count_{0};

public:
    using Pair    = chart_utils::Pair;
    using VecPair = chart_utils::VecPair;
    using VecComp = chart_utils::VecComp;

    explicit ChartPanel(const QString& title, const QString& titleX, const QString& titleY,
                        QWidget* parent = nullptr);

    [[nodiscard]] QChart* chart() const { return chart_; }
    [[nodiscard]] QChartView* view() const { return view_; }
    [[nodiscard]] std::size_t curveCount() const { return curve_count_; }

    void setChartTitle(const QString& title);

    void addRealCurve(const VecPair& points, const QString& name);
    void addComplexCurve(const VecComp& points, const QString& name);

    /// In-place update of the last data curve (slider / replace).
    bool replaceLastRealCurve(const VecPair& points, const QString& name);
    bool replaceLastComplexCurve(const VecComp& points, const QString& name);

    void clearCurves();

    /// Nice outward ranges (1–2–5×10^k), always include (0,0); draw origin cross.
    void fitAxes(double minX, double maxX, double minY, double maxY, bool padY = true);
};

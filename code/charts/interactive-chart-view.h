#pragma once

#include "code/charts/utils/chart-utils.hpp"

#include <QChartView>
#include <QPoint>

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

namespace chart_viewer {

/// Chart view with zoom-rectangle, pan, wheel zoom and coordinate readout.
class InteractiveChartView : public QChartView {
    Q_OBJECT

public:
    enum class Tool {
        ZoomRect, ///< LMB drag: rubber-band zoom (default)
        Pan,      ///< LMB drag: pan
    };

private:
    Tool tool_{Tool::ZoomRect};
    bool panning_{false};
    QPoint last_pos_{};
    bool grid_on_{true};

    void apply_tool_cursor();
    void sync_guides_to_axes();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

public:
    explicit InteractiveChartView(QChart* chart, QWidget* parent = nullptr);

    void setTool(Tool tool);
    [[nodiscard]] Tool tool() const noexcept { return tool_; }

    void zoomInStep();
    void zoomOutStep();
    void resetView(const chart_utils::Pair& home_x, const chart_utils::Pair& home_y);
    void setGridVisible(bool on);
    [[nodiscard]] bool isGridVisible() const noexcept { return grid_on_; }

signals:
    void cursorMoved(double x, double y, bool inside);
    void viewChanged();
};

} // namespace chart_viewer

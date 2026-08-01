#include "code/charts/interactive-chart-view.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QValueAxis>
#include <QWheelEvent>

namespace {

void apply_axis_grid(QChart* chart, bool on) {
    if (auto* ax = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0, nullptr))) {
        ax->setGridLineVisible(on);
        ax->setMinorGridLineVisible(on);
    }
    if (auto* ay = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).value(0, nullptr))) {
        ay->setGridLineVisible(on);
        ay->setMinorGridLineVisible(on);
    }
}

} // namespace

namespace chart_viewer {

InteractiveChartView::InteractiveChartView(QChart* chart, QWidget* parent) : QChartView(chart, parent) {
    setRenderHint(QPainter::Antialiasing, true);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setRubberBand(QChartView::RectangleRubberBand);
    apply_tool_cursor();
    apply_axis_grid(chart, grid_on_);
}

void InteractiveChartView::apply_tool_cursor() {
    switch (tool_) {
    case Tool::Pan:
        setCursor(Qt::OpenHandCursor);
        setRubberBand(QChartView::NoRubberBand);
        break;
    case Tool::ZoomRect:
        setCursor(Qt::CrossCursor);
        setRubberBand(QChartView::RectangleRubberBand);
        break;
    }
}

void InteractiveChartView::setTool(Tool tool) {
    tool_    = tool;
    panning_ = false;
    apply_tool_cursor();
}

void InteractiveChartView::sync_guides_to_axes() {
    auto* chart = this->chart();
    if (!chart)
        return;
    auto* ax = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).value(0, nullptr));
    auto* ay = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).value(0, nullptr));
    if (!ax || !ay)
        return;
    chart_utils::updateOriginGuides(chart, {ax->min(), ax->max()}, {ay->min(), ay->max()});
}

void InteractiveChartView::zoomInStep() {
    chart()->zoom(0.8);
    sync_guides_to_axes();
    emit viewChanged();
}

void InteractiveChartView::zoomOutStep() {
    chart()->zoom(1.25);
    sync_guides_to_axes();
    emit viewChanged();
}

void InteractiveChartView::resetView(const chart_utils::Pair& home_x, const chart_utils::Pair& home_y) {
    chart_utils::updateAxes(chart(), home_x, home_y);
    emit viewChanged();
}

void InteractiveChartView::setGridVisible(bool on) {
    grid_on_ = on;
    apply_axis_grid(chart(), on);
}

void InteractiveChartView::mousePressEvent(QMouseEvent* event) {
    if (tool_ == Tool::Pan && event->button() == Qt::LeftButton) {
        panning_  = true;
        last_pos_ = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    if (event->button() == Qt::MiddleButton) {
        panning_  = true;
        last_pos_ = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QChartView::mousePressEvent(event);
}

void InteractiveChartView::mouseMoveEvent(QMouseEvent* event) {
    if (panning_) {
        const QPoint delta = event->pos() - last_pos_;
        last_pos_          = event->pos();
        chart()->scroll(-delta.x(), delta.y());
        sync_guides_to_axes();
        emit viewChanged();
        event->accept();
    } else {
        QChartView::mouseMoveEvent(event);
    }

    if (chart()) {
        const QPointF v     = chart()->mapToValue(event->pos());
        const bool inside   = chart()->plotArea().contains(event->pos());
        emit cursorMoved(v.x(), v.y(), inside);
    }
}

void InteractiveChartView::mouseReleaseEvent(QMouseEvent* event) {
    if (panning_ && (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton)) {
        panning_ = false;
        apply_tool_cursor();
        sync_guides_to_axes();
        emit viewChanged();
        event->accept();
        return;
    }
    QChartView::mouseReleaseEvent(event);
    sync_guides_to_axes();
    emit viewChanged();
}

void InteractiveChartView::wheelEvent(QWheelEvent* event) {
    if (!chart() || event->angleDelta().y() == 0) {
        QChartView::wheelEvent(event);
        return;
    }
    const QPointF pos    = event->position();
    const QPointF before = chart()->mapToValue(pos);
    const double factor  = event->angleDelta().y() > 0 ? 0.85 : 1.0 / 0.85;
    chart()->zoom(factor);
    const QPointF after = chart()->mapToValue(pos);
    auto* ax            = qobject_cast<QValueAxis*>(chart()->axes(Qt::Horizontal).value(0, nullptr));
    auto* ay            = qobject_cast<QValueAxis*>(chart()->axes(Qt::Vertical).value(0, nullptr));
    if (ax && ay) {
        const double dx = before.x() - after.x();
        const double dy = before.y() - after.y();
        ax->setRange(ax->min() + dx, ax->max() + dx);
        ay->setRange(ay->min() + dy, ay->max() + dy);
    }
    sync_guides_to_axes();
    emit viewChanged();
    event->accept();
}

void InteractiveChartView::mouseDoubleClickEvent(QMouseEvent* event) {
    QChartView::mouseDoubleClickEvent(event);
    emit viewChanged();
}

void InteractiveChartView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
        zoomInStep();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Minus) {
        zoomOutStep();
        event->accept();
        return;
    }
    QChartView::keyPressEvent(event);
}

} // namespace chart_viewer

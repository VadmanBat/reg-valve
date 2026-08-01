#include "code/widgets/double-slider.h"

#include <algorithm>
#include <QPaintEvent>

DoubleSlider::DoubleSlider(Qt::Orientation orientation, QWidget* parent) : QSlider(orientation, parent) {
    single_step_ = (max_ - min_) / static_cast<double>(intervals_);
    QSlider::setRange(0, intervals_);
    connect(this, &QSlider::valueChanged, this, &DoubleSlider::on_int_value_changed);
}

void DoubleSlider::setRange(double min, double max, int intervals) {
    const double old_value = value();
    min_                   = min;
    max_                   = max;
    intervals_             = std::max(1, intervals);
    QSlider::setRange(0, intervals_);
    single_step_ = (max_ - min_) / static_cast<double>(intervals_);
    setValue(old_value);
}

double DoubleSlider::value() const {
    return min_ + QSlider::value() * single_step_;
}

void DoubleSlider::setValue(double value) {
    if (max_ <= min_) {
        QSlider::setValue(0);
        return;
    }
    const double t = (value - min_) / (max_ - min_);
    QSlider::setValue(static_cast<int>(std::clamp(t, 0.0, 1.0) * intervals_ + 0.5));
}

void DoubleSlider::paintEvent(QPaintEvent* event) {
    // Value / limits shown in RegParameter row — keep the track clean.
    QSlider::paintEvent(event);
}

void DoubleSlider::on_int_value_changed(int) {
    emit doubleValueChanged(value());
}

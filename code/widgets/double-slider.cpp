#include "code/widgets/double-slider.h"

#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionSlider>

DoubleSlider::DoubleSlider(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent) {
    m_singleStep = (m_max - m_min) / m_intervals;
    QSlider::setRange(0, m_intervals);
    connect(this, &QSlider::valueChanged, this, &DoubleSlider::onIntValueChanged);
}

void DoubleSlider::setRange(double min, double max, int intervals) {
    m_min = min;
    m_max = max;
    const int new_value = static_cast<int>(double(QSlider::value()) / m_intervals * intervals);
    QSlider::setRange(0, m_intervals = intervals);
    m_singleStep = (m_max - m_min) / m_intervals;
    QSlider::setValue(new_value);
}

double DoubleSlider::value() const {
    return m_min + QSlider::value() * m_singleStep;
}

void DoubleSlider::setValue(double value) {
    QSlider::setValue(static_cast<int>((value - m_min) * m_intervals / (m_max - m_min)));
}

void DoubleSlider::paintEvent(QPaintEvent* event) {
    QSlider::paintEvent(event);
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    QPainter painter(this);
    const QRect rect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    const QRect textRect = rect.adjusted(-width() / 2, -55, width() / 2, height() / 2);
    painter.drawText(textRect, Qt::AlignCenter, QString::number(this->value(), 'f', 2));
    const QRect limitRect(0, 30, width(), height());
    painter.drawText(limitRect, Qt::AlignLeft, QString::number(m_min, 'f', 2));
    painter.drawText(limitRect, Qt::AlignRight, QString::number(m_max, 'f', 2));
}

void DoubleSlider::onIntValueChanged(int) {
    emit doubleValueChanged(value());
}

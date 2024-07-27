//
// Created by Vadma on 19.07.2024.
//

#ifndef REGVALVE_DOUBLESLIDER_HPP
#define REGVALVE_DOUBLESLIDER_HPP

#include <QApplication>
#include <QWidget>
#include <QSlider>
#include <QPainter>
#include <QStyleOptionSlider>

class DoubleSlider : public QSlider {
public:
    explicit DoubleSlider(Qt::Orientation orientation, QWidget* parent = nullptr) :
            QSlider(orientation, parent), m_min(0.0), m_max(1.0), m_intervals(100)
    {
        m_singleStep = (m_max - m_min) / m_intervals;
        connect(this, &QSlider::valueChanged, this, &DoubleSlider::onValueChanged);
    }

    void setRange(double min, double max, int intervals) {
        m_min = min;
        m_max = max;

        int new_value = static_cast<int>(double(QSlider::value()) / m_intervals * intervals);
        QSlider::setRange(0, m_intervals = intervals);
        m_singleStep = (m_max - m_min) / m_intervals;
        QSlider::setValue(new_value);
    }

    [[nodiscard]] double value() const {
        return m_min + QSlider::value() * m_singleStep;
    }

public slots:
    void setValue(double value) {
        QSlider::setValue(static_cast<int>((value - m_min) * m_intervals / (m_max - m_min)));
    }

signals:
    void doubleValueChanged(double value) {

    };

protected:
    void paintEvent(QPaintEvent* event) override {
        QSlider::paintEvent(event);

        QStyleOptionSlider opt;
        initStyleOption(&opt);

        QPainter painter(this);
        QRect rect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
        QRect textRect = rect.adjusted(-width() / 2, -55, width() / 2, height() / 2);
        painter.drawText(textRect, Qt::AlignCenter, QString::number(this->value(), 'f', 2));

        QRect limitRect = QRect(0, 30, width(), height());
        painter.drawText(limitRect, Qt::AlignLeft, QString::number(m_min, 'f', 2));
        painter.drawText(limitRect, Qt::AlignRight, QString::number(m_max, 'f', 2));
    }

private slots:
    void onValueChanged(int value) {
        emit doubleValueChanged(this->value());
    }

private:
    double m_min;
    double m_max;
    double m_singleStep;

    int m_intervals;
};

#endif //REGVALVE_DOUBLESLIDER_HPP

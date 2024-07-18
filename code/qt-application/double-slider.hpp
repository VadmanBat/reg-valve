//
// Created by Vadma on 19.07.2024.
//

#ifndef REGVALVE_DOUBLESLIDER_HPP
#define REGVALVE_DOUBLESLIDER_HPP

#include <QApplication>
#include <QWidget>

class DoubleSlider : public QSlider {
public:
    explicit DoubleSlider(Qt::Orientation orientation, QWidget *parent = nullptr) :
            QSlider(orientation, parent), m_min(0.0), m_max(1.0), m_singleStep(0.01)
    {
        connect(this, &QSlider::valueChanged, this, &DoubleSlider::onValueChanged);
    }

    void setRange(double min, double max) {
        m_min = min;
        m_max = max;
        QSlider::setRange(0, 1000);
    }

    void setSingleStep(double step) {
        m_singleStep = step;
        QSlider::setSingleStep(static_cast<int>(step * 1000 / (m_max - m_min)));
    }

    double value() const {
        return QSlider::value() * (m_max - m_min) / 1000.0 + m_min;
    }

public slots:
    void setValue(double value) {
        QSlider::setValue(static_cast<int>((value - m_min) * 1000 / (m_max - m_min)));
    }

signals:
    void doubleValueChanged(double value) {

    };

protected:
    void paintEvent(QPaintEvent *event) override {
        QSlider::paintEvent(event);

        QStyleOptionSlider opt;
        initStyleOption(&opt);

        QPainter painter(this);
        QRect rect = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
        QString valueText = QString::number(this->value(), 'f', 2);

        // Рисуем текст над ползунком, перекрывая его
        QRect textRect = rect.adjusted(-rect.width() / 2, -rect.height() * 2, rect.width() / 2, -rect.height());
        painter.drawText(textRect, Qt::AlignCenter, valueText);

        // Рисуем минимум и максимум
        QString minText = QString::number(m_min, 'f', 2);
        QString maxText = QString::number(m_max, 'f', 2);
        QRect minRect = rect.adjusted(-width() / 2, height() / 2, -width() / 2 + 50, height() / 2 + 20);
        QRect maxRect = rect.adjusted(width() / 2 - 50, height() / 2, width() / 2, height() / 2 + 20);
        painter.drawText(minRect, Qt::AlignLeft, minText);
        painter.drawText(maxRect, Qt::AlignRight, maxText);

        // Рисуем засечки
        int tickInterval = this->tickInterval();
        if (tickInterval > 0) {
            for (int i = 0; i <= this->maximum(); i += tickInterval) {
                int x = QStyle::sliderPositionFromValue(this->minimum(), this->maximum(), i, this->width());
                painter.drawLine(x, rect.bottom() + 5, x, rect.bottom() + 15);
            }
        }
    }

private slots:
    void onValueChanged(int value) {
        emit doubleValueChanged(this->value());
    }

private:
    double m_min;
    double m_max;
    double m_singleStep;
};

#endif //REGVALVE_DOUBLESLIDER_HPP

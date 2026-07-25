#pragma once

#include <QSlider>

class DoubleSlider : public QSlider {
    Q_OBJECT
public:
    explicit DoubleSlider(Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = nullptr);

    void setRange(double min, double max, int intervals);
    [[nodiscard]] double value() const;

public slots:
    void setValue(double value);

signals:
    void doubleValueChanged(double value);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onIntValueChanged(int);

private:
    double m_min{0.0};
    double m_max{1.0};
    double m_singleStep{0.01};
    int m_intervals{100};
};

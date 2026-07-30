#pragma once

#include <QSlider>

class DoubleSlider : public QSlider {
    Q_OBJECT

private:
    double min_{0.0};
    double max_{1.0};
    double single_step_{0.01};
    int intervals_{100};

private slots:
    void on_int_value_changed(int);

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
};

#pragma once

#include "code/widgets/double_slider.h"

#include <QObject>
#include <QString>

class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QLayout;
class QSpinBox;

class RegParameter : public QObject {
    Q_OBJECT
public:
    RegParameter(const QString& title, double min, double max, double minValue, double maxValue,
                 QObject* parent = nullptr);

    [[nodiscard]] QLayout* layout() const { return layout_; }
    [[nodiscard]] QCheckBox* checkBox() const { return checkBox_; }
    [[nodiscard]] DoubleSlider* slider() const { return slider_; }
    [[nodiscard]] bool enabled() const;
    [[nodiscard]] double value() const;

private slots:
    void updateSliderRange();
    void enable(bool checked);

private:
    void applyDefaultStyle();

    QLabel* label_{nullptr};
    QCheckBox* checkBox_{nullptr};
    QDoubleSpinBox *minSpinBox_{nullptr}, *maxSpinBox_{nullptr};
    QSpinBox* pointsSpinBox_{nullptr};
    DoubleSlider* slider_{nullptr};
    QLayout* layout_{nullptr};
};

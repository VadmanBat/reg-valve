#pragma once

#include "code/widgets/double-slider.h"

#include <QObject>
#include <QString>

class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QLayout;
class QSpinBox;

class RegParameter : public QObject {
    Q_OBJECT

private:
    QLabel* label_{nullptr};
    QCheckBox* check_box_{nullptr};
    QDoubleSpinBox* min_spin_{nullptr};
    QDoubleSpinBox* max_spin_{nullptr};
    QSpinBox* points_spin_{nullptr};
    DoubleSlider* slider_{nullptr};
    QLayout* layout_{nullptr};

    void apply_default_style();

private slots:
    void update_slider_range();
    void enable(bool checked);

public:
    RegParameter(const QString& title, double min, double max, double minValue, double maxValue,
                 QObject* parent = nullptr);

    [[nodiscard]] QLayout* layout() const { return layout_; }
    [[nodiscard]] QCheckBox* checkBox() const { return check_box_; }
    [[nodiscard]] DoubleSlider* slider() const { return slider_; }
    [[nodiscard]] bool enabled() const;
    [[nodiscard]] double value() const;
};

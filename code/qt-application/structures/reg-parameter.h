//
// Created by Vadma on 23.07.2024.
//

#ifndef REGVALVE_REG_PARAMETER_H
#define REGVALVE_REG_PARAMETER_H

#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QObject>

#include "double-slider.hpp"

class RegParameter : public QObject {
private:
    QLabel* label;
    QCheckBox* checkBox;
    QDoubleSpinBox *minSpinBox, *maxSpinBox;
    QSpinBox* intervalsSpinBox;
    DoubleSlider* slider;

    QLayout* layout;

    static void setSpinBox(QDoubleSpinBox* spinBox, double min, double max, double value, const QString& prefix) {
        spinBox->setRange(min, max);
        spinBox->setDecimals(2);
        spinBox->setValue(value);
        spinBox->setPrefix(prefix);
    }

    void updateSliderRange() {
        auto min = minSpinBox->value();
        auto max = maxSpinBox->value();
        auto intervals = intervalsSpinBox->value();

        if (min < max) {
            auto value = slider->value();
            slider->setRange(min, max, intervals);
            slider->setValue(value);
        }
    }

    void enable(bool checked) {
        minSpinBox->setEnabled(checked);
        maxSpinBox->setEnabled(checked);
        intervalsSpinBox->setEnabled(checked);
        slider->setEnabled(checked);
    }

    void applyDefaultStyle() {
        label->setStyleSheet("font-size: 16pt;");
        minSpinBox->setStyleSheet("font-size: 9pt;");
        maxSpinBox->setStyleSheet("font-size: 9pt;");
        intervalsSpinBox->setStyleSheet("font-size: 9pt;");

        minSpinBox->setFixedSize(100, 30);
        maxSpinBox->setFixedSize(100, 30);
        intervalsSpinBox->setFixedSize(100, 30);
    }

public:
    RegParameter(const QString& title, double min, double max, double minValue, double maxValue) :
            label(new QLabel(title)),
            checkBox(new QCheckBox),
            minSpinBox(new QDoubleSpinBox), maxSpinBox(new QDoubleSpinBox), intervalsSpinBox(new QSpinBox),
            slider(new DoubleSlider(Qt::Orientation::Horizontal)),
            layout(new QHBoxLayout)
    {
        setSpinBox(minSpinBox, min, max, minValue, "от: ");
        setSpinBox(maxSpinBox, min, max, maxValue, "до: ");
        intervalsSpinBox->setRange(10, 1000);
        intervalsSpinBox->setPrefix("точки: ");
        intervalsSpinBox->setValue(100);

        updateSliderRange();
        enable(false);

        connect(minSpinBox, &QDoubleSpinBox::editingFinished, [this] {
            if (minSpinBox->value() > maxSpinBox->value())
                maxSpinBox->setValue(minSpinBox->value());
            else
                updateSliderRange();
        });
        connect(maxSpinBox, &QDoubleSpinBox::editingFinished, [this] {
            if (minSpinBox->value() > maxSpinBox->value())
                minSpinBox->setValue(maxSpinBox->value());
            else
                updateSliderRange();
        });
        connect(intervalsSpinBox, &QSpinBox::editingFinished, this, &RegParameter::updateSliderRange);
        connect(checkBox, &QCheckBox::toggled, this, &RegParameter::enable);

        layout->addWidget(label);
        layout->addWidget(checkBox);
        layout->addWidget(minSpinBox);
        layout->addWidget(maxSpinBox);
        layout->addWidget(intervalsSpinBox);
        layout->addWidget(slider);

        applyDefaultStyle();
    }

    [[nodiscard]] QLayout* getLayout() const { return layout; }
    [[nodiscard]] QCheckBox* getCheckBox() const { return checkBox; }
    [[nodiscard]] DoubleSlider* getSlider() const { return slider; }
};

#endif //REGVALVE_REG_PARAMETER_H

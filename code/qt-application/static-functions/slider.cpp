//
// Created by Vadma on 21.07.2024.
//
#include "../application.h"

void Application::updateSliderRange(QDoubleSpinBox *minSpinBox, QDoubleSpinBox *maxSpinBox, QSpinBox *pointsSpinBox, DoubleSlider *slider) {
    double min = minSpinBox->value();
    double max = maxSpinBox->value();
    int points = pointsSpinBox->value();

    if (min < max && points > 0) {
        slider->setRange(min, max);
        slider->setSingleStep((max - min) / (points - 1));
    }
}

void Application::setSpinBox(QDoubleSpinBox* spinBox, double min, double max, double value, const char* prefix) {
    spinBox->setRange(min, max);
    spinBox->setDecimals(2);
    spinBox->setValue(value);
    spinBox->setPrefix(prefix);
}

std::pair <DoubleSlider*, QLayout*> Application::createSliderForm(const SliderData& data) {
    const auto [title, lower, upper, min, max] = data;

    auto layout = new QHBoxLayout;
    auto titleLabel = new QLabel(title);
    titleLabel->setStyleSheet("font-size: 16pt;");
    layout->addWidget(titleLabel);

    auto enableCheckBox = new QCheckBox("Enable");
    layout->addWidget(enableCheckBox);

    auto minSpinBox = new QDoubleSpinBox;
    auto maxSpinBox = new QDoubleSpinBox;
    setSpinBox(minSpinBox, lower, upper, min, "min: ");
    setSpinBox(maxSpinBox, lower, upper, max, "max: ");
    layout->addWidget(minSpinBox);
    layout->addWidget(maxSpinBox);

    auto pointsSpinBox = new QSpinBox;
    pointsSpinBox->setRange(10, 1000);
    pointsSpinBox->setPrefix("Points: ");
    pointsSpinBox->setValue(50);
    layout->addWidget(pointsSpinBox);

    auto slider = new DoubleSlider(Qt::Horizontal);
    layout->addWidget(slider);

    connect(minSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double) {
        updateSliderRange(minSpinBox, maxSpinBox, pointsSpinBox, slider);
        if (minSpinBox->value() > maxSpinBox->value())
            maxSpinBox->setValue(minSpinBox->value());
    });
    connect(maxSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double) {
        updateSliderRange(minSpinBox, maxSpinBox, pointsSpinBox, slider);
        if (minSpinBox->value() > maxSpinBox->value())
            minSpinBox->setValue(maxSpinBox->value());
    });
    connect(pointsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int) {
        updateSliderRange(minSpinBox, maxSpinBox, pointsSpinBox, slider);
    });

    connect(slider, &QSlider::valueChanged, [=](int value) {

    });

    updateSliderRange(minSpinBox, maxSpinBox, pointsSpinBox, slider);

    connect(enableCheckBox, &QCheckBox::toggled, [=](bool checked) {
        minSpinBox->setEnabled(checked);
        maxSpinBox->setEnabled(checked);
        pointsSpinBox->setEnabled(checked);
        slider->setEnabled(checked);
    });

    enableCheckBox->setChecked(false);
    minSpinBox->setEnabled(false);
    maxSpinBox->setEnabled(false);
    pointsSpinBox->setEnabled(false);
    slider->setEnabled(false);

    return {slider, layout};
}

QLayout* Application::createSlidersForm(std::vector <DoubleSlider*>& sliders, const SlidersDataset& dataset) {
    sliders.reserve(dataset.size());
    auto layout = new QVBoxLayout;
    for (const auto& sliderData : dataset) {
        auto [slider, sliderLayout](createSliderForm(sliderData));
        sliders.emplace_back(slider);
        layout->addLayout(sliderLayout);
    }
    return layout;
}
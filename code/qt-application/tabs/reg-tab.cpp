//
// Created by Vadma on 18.07.2024.
//
#include "../application.h"

QWidget* Application::createRegTab() {
    auto transferFunctionLayout = createTransferFunctionForm(regNumerator, regDenominator, 6, 6, "W<sub>ОУ</sub>(p) = ");

    auto slidersLayout = createSlidersForm(regSliders, REG_SLIDERS);

    for (auto slider : regSliders)
        connect(slider, &QSlider::valueChanged, this, &Application::regReplaceTransferFunction);

    auto addButton = new QPushButton("Добавить");
    connect(addButton, &QPushButton::clicked, this, &Application::regAddTransferFunction);

    auto regTab = new QWidget;

    auto chartsLayout = createCharts(REG_CHARTS, regTab);

    auto resLayout = new QVBoxLayout;
    resLayout->addLayout(transferFunctionLayout);
    resLayout->addLayout(slidersLayout);
    resLayout->addWidget(addButton);
    resLayout->addLayout(chartsLayout);

    regTab->setLayout(resLayout);
    return regTab;
}

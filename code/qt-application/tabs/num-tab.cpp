//
// Created by Vadma on 08.07.2024.
//
#include "../application.h"

QWidget* Application::createNumTab() {
    auto numTab = new QWidget(this);
    auto layout = new QVBoxLayout(numTab);

    auto transferFunctionLayout = createTransferFunctionForm(numNumerator, numDenominator);

    numWidget->setLabels({
                                 "t<sub>р</sub>:", "ω<sub>n</sub>:",
                                 "t<sub>н</sub>:", "ω<sub>c</sub>:",
                                 "ζ:", "h<sub>уст</sub>:"
                         });
    numWidget->setPrecisions({
                                     2, 4,
                                     2, 4,
                                     4, 2
                             });
    numWidget->setColors({
                                 {0, 0}, {0, 0},
                                 {0, 0}, {0, 0},
                                 {0, 0}, {0, 0},
                         });

    auto uppLayout = new QHBoxLayout;
    uppLayout->addLayout(transferFunctionLayout);
    uppLayout->addWidget(numWidget);

    auto modParButton   = new QPushButton("Параметры моделирования", numTab);
    auto addButton      = new QPushButton("Добавить", numTab);
    auto replaceButton  = new QPushButton("Заменить", numTab);
    auto clearButton    = new QPushButton("Очистить", numTab);

    connect(modParButton, &QPushButton::clicked, this, &Application::numShowModParDialog);
    connect(addButton, &QPushButton::clicked, this, &Application::numAddTransferFunction);
    connect(replaceButton, &QPushButton::clicked, this, &Application::numReplaceTransferFunction);
    connect(clearButton, &QPushButton::clicked, this, &Application::numClearCharts);

    auto buttonLayout = new QHBoxLayout;
    auto rightButtonLayout = new QVBoxLayout;
    buttonLayout->addWidget(modParButton);
    rightButtonLayout->addWidget(addButton);
    rightButtonLayout->addWidget(replaceButton);
    rightButtonLayout->addWidget(clearButton);
    buttonLayout->addLayout(rightButtonLayout);

    layout->addLayout(uppLayout);
    layout->addLayout(buttonLayout);
    layout->addLayout(createCharts(NUM_CHARTS, numTab));

    numTab->setLayout(layout);
    return numTab;
}

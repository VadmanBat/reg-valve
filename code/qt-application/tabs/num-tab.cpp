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
                                 "ζ:", "y<sub>уст</sub>:"
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

    auto addButton      = new QPushButton("Добавить", numTab);
    auto replaceButton  = new QPushButton("Заменить", numTab);
    auto clearButton    = new QPushButton("Очистить", numTab);

    connect(addButton, &QPushButton::clicked, this, &Application::numAddTransferFunction);
    connect(replaceButton, &QPushButton::clicked, this, &Application::numReplaceTransferFunction);
    connect(clearButton, &QPushButton::clicked, this, &Application::numClearCharts);

    layout->addLayout(uppLayout);
    layout->addWidget(addButton);
    layout->addWidget(replaceButton);
    layout->addWidget(clearButton);
    layout->addLayout(createCharts(NUM_CHARTS, numTab));

    numTab->setLayout(layout);
    return numTab;
}

//
// Created by Vadim on 08.07.2024.
//
#include "../application.h"

QWidget* Application::createNumTab() {
    auto numTab = new QWidget(this);
    auto layout = new QVBoxLayout(numTab);

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
    uppLayout->addLayout(numTF.getLayout(), 80);
    uppLayout->addWidget(numWidget, 20);

    auto queButton      = new QPushButton(QChar(0xf128), numTab);
    auto setButton      = new QPushButton(QChar(0xf013), numTab);
    auto addButton      = new QPushButton(QChar(0xf067), numTab);
    auto replaceButton  = new QPushButton(QChar(0xf021), numTab);
    auto clearButton    = new QPushButton(QChar(0xf00d), numTab);

    queButton->setToolTip("Описание типовых звеньев");
    setButton->setToolTip("Параметры моделирования");
    addButton->setToolTip("Добавить передаточную функцию");
    replaceButton->setToolTip("Заменить последнюю передаточную функцию");
    clearButton->setToolTip("Очистить все передаточные функции");

    queButton->setFont(font);
    setButton->setFont(font);
    addButton->setFont(font);
    replaceButton->setFont(font);
    clearButton->setFont(font);

    auto nameLabel = new QLabel;

    numTF.setTF(numTranFunc);
    regTF.setTF(regTranFunc);
    numTF.createConnect(nameLabel);
    connect(setButton, &QPushButton::clicked, [this](){
        showModParDialog(numModelParam);
    });
    connect(addButton,      &QPushButton::clicked, this, &Application::numAddTransferFunction);
    connect(replaceButton,  &QPushButton::clicked, this, &Application::numReplaceTransferFunction);
    connect(clearButton,    &QPushButton::clicked, this, &Application::numClearCharts);

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(nameLabel);
    buttonLayout->addWidget(queButton);
    buttonLayout->addWidget(setButton);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(replaceButton);
    buttonLayout->addWidget(clearButton);

    layout->addLayout(uppLayout);
    layout->addLayout(buttonLayout);
    layout->addLayout(createCharts(NUM_CHARTS, numTab));

    numTab->setLayout(layout);
    return numTab;
}

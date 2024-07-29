//
// Created by Vadma on 18.07.2024.
//
#include "../application.h"

QWidget* Application::createRegTab() {
    auto regTab = new QWidget(this);
    auto layout = new QVBoxLayout;

    auto transferFunctionLayout = createTransferFunctionForm(
            regNumerator, regDenominator, 6, 6, "W<sub>ОУ</sub>(p) = "
    );

    regParameters.assign({
                                 new RegParameter("K<sub>p</sub>", 0.05, 50, 1, 10),
                                 new RegParameter("T<sub>u</sub>", 0.05, 2000, 1, 120),
                                 new RegParameter("T<sub>d</sub>", 0.05, 2000, 1, 60)
                         });
    auto parametersLayout = new QVBoxLayout;
    for (auto parameter : regParameters) {
        parametersLayout->addLayout(parameter->getLayout());
        connect(parameter->getCheckBox(), &QCheckBox::stateChanged, this, &Application::regReplaceTransferFunction);
        connect(parameter->getSlider(), &QSlider::valueChanged, this, &Application::regReplaceTransferFunction);
    }
    regWidget->setLabels({
                                 "t<sub>р</sub>:", "ω<sub>n</sub>:", "y<sub>уст</sub>:", "ЛИК:",
                                 "t<sub>н</sub>:", "ω<sub>c</sub>:", "σ<sub>ст</sub>:", "ИКК:",
                                 "t<sub>п</sub>:", "ζ:", "σ<sub>пр</sub>:", "СКО:"
                         });
    regWidget->setPrecisions({
                                     2, 4, 2, 4,
                                     2, 4, 2, 4,
                                     2, 4, 2, 4
                             });
    regWidget->setColors({
                                 {1, 2}, {0, 0}, {0, 0}, {1, 2},
                                 {1, 2}, {0, 0}, {0, 0}, {1, 2},
                                 {1, 2}, {2, 1}, {1, 2}, {1, 2},
                         });

    auto uppLayout = new QHBoxLayout;
    uppLayout->addLayout(transferFunctionLayout);
    uppLayout->addWidget(regWidget);

    auto addButton = new QPushButton("Добавить", regWidget);
    connect(addButton, &QPushButton::clicked, this, &Application::regAddTransferFunction);
    /*QPushButton *fullScreenButton = new QPushButton("Full Screen", regWidget);

    auto x = createCharts(REG_CHARTS, regTab);
    auto y = qobject_cast<QChartView*>(x->itemAt(0)->widget());

    connect(fullScreenButton, &QPushButton::clicked, [=, this](){
        toggleFullScreen(y);
    });*/

    layout->addLayout(uppLayout);
    layout->addLayout(parametersLayout);
    layout->addWidget(addButton);
    //layout->addWidget(fullScreenButton);
    layout->addLayout(x);

    regTab->setLayout(layout);
    return regTab;
}

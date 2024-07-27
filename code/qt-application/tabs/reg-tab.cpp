//
// Created by Vadma on 18.07.2024.
//
#include "../application.h"

QWidget* Application::createRegTab() {
    auto transferFunctionLayout = createTransferFunctionForm(
            regNumerator, regDenominator, 6, 6, "W<sub>ОУ</sub>(p) = "
    );

    regParameters.assign({
                                 new RegParameter("K<sub>p</sub>", 0.1, 50, 1, 10),
                                 new RegParameter("T<sub>u</sub>", 0.1, 2000, 1, 120),
                                 new RegParameter("T<sub>d</sub>", 0.1, 2000, 1, 60)
                         });
    auto parametersLayout = new QVBoxLayout;
    for (auto parameter : regParameters) {
        parametersLayout->addLayout(parameter->getLayout());
        connect(parameter->getCheckBox(), &QCheckBox::stateChanged, this, &Application::regReplaceTransferFunction);
        connect(parameter->getSlider(), &QSlider::valueChanged, this, &Application::regReplaceTransferFunction);
    }

    auto uppLayout = new QHBoxLayout;
    uppLayout->addLayout(transferFunctionLayout);
    uppLayout->addWidget(regWidget);

    auto addButton = new QPushButton("Добавить");
    connect(addButton, &QPushButton::clicked, this, &Application::regAddTransferFunction);

    auto regTab = new QWidget(this);

    auto layout = new QVBoxLayout;
    layout->addLayout(uppLayout);
    layout->addLayout(parametersLayout);
    layout->addWidget(addButton);
    layout->addLayout(createCharts(REG_CHARTS, regTab));

    regTab->setLayout(layout);
    return regTab;
}

//
// Created by Vadma on 18.07.2024.
//
#include "../application.h"
#include "../structures/dialogs/help-window.hpp"

QWidget* Application::createRegTab() {
    auto regTab = new QWidget(this);
    auto layout = new QVBoxLayout;

    auto transferFunctionLayout = createTransferFunctionForm(
            regNumerator, regDenominator, 6, 6, "W<sub>ОУ</sub>(p) = "
    );

    regParameters.assign({
                                 new RegParameter("K<sub>p</sub>", 0.05, 50, 0.05, 5),
                                 new RegParameter("T<sub>u</sub>", 0.05, 2000, 1, 120),
                                 new RegParameter("T<sub>d</sub>", 0.05, 2000, 1, 60)
                         });
    auto parametersLayout = new QVBoxLayout;
    for (auto parameter : regParameters) {
        parametersLayout->addLayout(parameter->getLayout());
        connect(parameter->getCheckBox(), &QCheckBox::toggled, this, &Application::regReplaceTransferFunction);
        connect(parameter->getSlider(), &QSlider::valueChanged, this, &Application::regReplaceTransferFunction);
    }
    regWidget->setLabels({
                                 "t<sub>р</sub>:", "ω<sub>n</sub>:", "h<sub>уст</sub>:", "ЛИК:",
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

    /*replay 0xf021*/
    auto queButton      = new QPushButton(QChar(0xf128), regWidget);
    auto setButton      = new QPushButton(QChar(0xf013), regWidget);

    auto addButton      = new QPushButton(QChar(0xf067), regWidget);
    auto clearButton    = new QPushButton(QChar(0xf00d), regWidget);

    queButton->setFont(font);
    setButton->setFont(font);

    addButton->setFont(font);
    clearButton->setFont(font);
    addButton->setStyleSheet(
            "QPushButton {"
            "background-color: green;"
            "color: white;"
            "border-style: outset;"
            "border-width: 2px;"
            "border-radius: 5px;"
            "padding: 5px;" // Добавили отступы для комфортного размера
            "}"
            "QPushButton:hover {"
            "background-color: darkgreen;"
            "border-style: inset;" // Изменяем стиль рамки при наведении
            "}"
            "QPushButton:pressed {"
            "background-color: darkseagreen;"
            "border-style: inset;" // Изменяем стиль рамки при нажатии
            "padding: 3px;" // Уменьшаем отступ при нажатии
            "}"
    );

    clearButton->setStyleSheet(
            "QPushButton {"
            "background-color: red;"
            "color: white;"
            "border-style: outset;"
            "border-width: 2px;"
            "border-radius: 5px;"
            "padding: 5px;"
            "}"
            "QPushButton:hover {"
            "background-color: darkred;"
            "border-style: inset;"
            "}"
            "QPushButton:pressed {"
            "background-color: crimson;"
            "border-style: inset;"
            "padding: 3px;"
            "}"
    );

    QFile qssFile("styles/button-style.qss");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(qssFile.readAll());
    qssFile.close();

    queButton->setFixedSize(30,30);
    setButton->setFixedSize(30,30);

    addButton->setFixedSize(40,40);
    clearButton->setFixedSize(40,40);

    addButton->setObjectName("addButton");
    clearButton->setObjectName("clearButton");
    addButton->setStyleSheet(styleSheet);
    clearButton->setStyleSheet(styleSheet);

    connect(queButton, &QPushButton::clicked, [](){
        HelpWindow().exec();
    });
    connect(setButton, &QPushButton::clicked, [this](){
        showModParDialog(regModelParam);
    });
    connect(addButton, &QPushButton::clicked, this, &Application::regAddTransferFunction);
    connect(clearButton, &QPushButton::clicked, this, &Application::regClearCharts);

    auto buttonLayout = new QVBoxLayout;
    auto buttonUppLayout = new QVBoxLayout;

    buttonUppLayout->addWidget(queButton);
    buttonUppLayout->addWidget(setButton);
    uppLayout->addLayout(buttonUppLayout);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(clearButton);

    auto t_layout = new QHBoxLayout;
    t_layout->addLayout(parametersLayout);
    t_layout->addLayout(buttonLayout);

    layout->addLayout(uppLayout);
    //layout->addLayout(parametersLayout);
    //layout->addLayout(buttonLayout);
    layout->addLayout(t_layout);
    layout->addLayout(createCharts(REG_CHARTS, regTab));

    regTab->setLayout(layout);
    return regTab;
}

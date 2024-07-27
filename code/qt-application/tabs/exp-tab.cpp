//
// Created by Vadma on 08.07.2024.
//

#include "../application.h"

using namespace QtCharts;

QWidget* Application::createExpTab() {
    auto expTab = new QWidget(this);
    auto layout = new QVBoxLayout(expTab);

    auto openFileButton = new QPushButton("Открыть файл", expTab);
    connect(openFileButton, &QPushButton::clicked, this, &Application::expOpenFile);

    layout->addWidget(openFileButton);
    layout->addLayout(createCharts(EXP_CHARTS, expTab));

    expTab->setLayout(layout);
    return expTab;
}

//
// Created by Vadma on 08.07.2024.
//

#include "../application.h"

using namespace QtCharts;

QWidget* Application::createExpTab() {
    QWidget *expTab = new QWidget();
    QVBoxLayout *resLayout = new QVBoxLayout(expTab);
    QPushButton *openFileButton = new QPushButton("Открыть файл", expTab);
    connect(openFileButton, &QPushButton::clicked, this, &Application::openFile);
    resLayout->addWidget(openFileButton);

    QHBoxLayout* chartsLayout = createCharts(EXP_CHARTS, expTab);
    resLayout->addLayout(chartsLayout);

    expTab->setLayout(resLayout);
    return expTab;
}
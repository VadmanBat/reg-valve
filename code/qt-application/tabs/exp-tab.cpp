//
// Created by Vadma on 08.07.2024.
//

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtCharts>

#include "../application.h"

using namespace QtCharts;

QWidget* GraphWindow::createExpTab() {
    QWidget *expTab = new QWidget();
    QVBoxLayout *graphLayout = new QVBoxLayout(expTab);
    QPushButton *openFileButton = new QPushButton("Открыть файл", expTab);
    connect(openFileButton, &QPushButton::clicked, this, &GraphWindow::openFile);
    graphLayout->addWidget(openFileButton);

    chart = new QChart();
    chartView = new QChartView(chart, expTab);
    graphLayout->addWidget(chartView);

    expTab->setLayout(graphLayout);
    return expTab;
}
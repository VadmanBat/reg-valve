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
    QWidget *graphTab = new QWidget();
    QVBoxLayout *graphLayout = new QVBoxLayout(graphTab);
    QPushButton *openFileButton = new QPushButton("Открыть файл", graphTab);
    connect(openFileButton, &QPushButton::clicked, this, &GraphWindow::openFile);
    graphLayout->addWidget(openFileButton);

    chart = new QChart();
    chartView = new QChartView(chart, graphTab);
    graphLayout->addWidget(chartView);

    graphTab->setLayout(graphLayout);
    return graphTab;
}
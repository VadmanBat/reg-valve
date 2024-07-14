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
    QVBoxLayout *resLayout = new QVBoxLayout(expTab);
    QPushButton *openFileButton = new QPushButton("Открыть файл", expTab);
    connect(openFileButton, &QPushButton::clicked, this, &GraphWindow::openFile);
    resLayout->addWidget(openFileButton);

    expChartTranResp = new QChart();
    expChartFreqResp = new QChart();

    expChartTranResp->setTitle("Переходная характеристика");
    expChartFreqResp->setTitle("Комплексно-частотная характеристика (КЧХ)");
    createAxes(expChartTranResp, "Ось времени", "Ось значений");
    createAxes(expChartFreqResp, "Реальная ось", "Мнимая ось");

    expChartTranRespView = new QChartView(expChartTranResp, expTab);
    expChartFreqRespView = new QChartView(expChartFreqResp, expTab);

    QHBoxLayout* chartsLayout = new QHBoxLayout;
    chartsLayout->addWidget(expChartTranRespView);
    chartsLayout->addWidget(expChartFreqRespView);
    resLayout->addLayout(chartsLayout);

    expTab->setLayout(resLayout);
    return expTab;
}
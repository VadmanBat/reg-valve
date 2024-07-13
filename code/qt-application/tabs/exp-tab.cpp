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

    expChartH = new QChart();
    expChartFreqResp = new QChart();

    setChart(expChartH,
             "Переходная характеристика",
             "Ось времени", "Ось значений"
    );
    setChart(expChartFreqResp,
             "Комплексно-частотная характеристика (КЧХ)",
             "Реальная ось", "Мнимая ось"
    );

    expChartHView = new QChartView(expChartH, expTab);
    expChartFreqRespView = new QChartView(expChartFreqResp, expTab);

    QHBoxLayout* chartsLayout = new QHBoxLayout;
    chartsLayout->addWidget(expChartHView);
    chartsLayout->addWidget(expChartFreqRespView);
    resLayout->addLayout(chartsLayout);

    expTab->setLayout(resLayout);
    return expTab;
}
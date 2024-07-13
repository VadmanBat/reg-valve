//
// Created by Vadma on 08.07.2024.
//
#include "../application.h"

QWidget* GraphWindow::createNumTab() {
    QLabel *transferFunctionLabel = new QLabel("W<sub>ОУ</sub>(p) = ");
    transferFunctionLabel->setAlignment(Qt::AlignCenter);
    transferFunctionLabel->setStyleSheet("font-size: 24pt;");

    QHBoxLayout *numeratorLayout = new QHBoxLayout;
    QHBoxLayout *denominatorLayout = new QHBoxLayout;

    QDoubleValidator *realNumberValidator = new QDoubleValidator(this);
    realNumberValidator->setNotation(QDoubleValidator::StandardNotation);

    createLineEdit("Числитель:\t", numeratorLayout, realNumberValidator);
    createLineEdit("Знаменатель:\t", denominatorLayout, realNumberValidator);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setMidLineWidth(10);

    QVBoxLayout *transferFunctionLayout = new QVBoxLayout();
    transferFunctionLayout->addLayout(numeratorLayout);
    transferFunctionLayout->addWidget(line);
    transferFunctionLayout->addLayout(denominatorLayout);

    QHBoxLayout *uppLayout = new QHBoxLayout;
    uppLayout->addWidget(transferFunctionLabel);
    uppLayout->addLayout(transferFunctionLayout);

    QPushButton *calculateButton = new QPushButton("Рассчитать");
    connect(calculateButton, &QPushButton::clicked, [this, numeratorLayout, denominatorLayout] {
        auto numeratorData = getLineEditData(numeratorLayout);
        auto denominatorData = getLineEditData(denominatorLayout);

        std::vector <double> time;
        for (double t = 0.00; t < 60; t += 0.01)
            time.push_back(t);

        std::vector <double> omega;
        for (double w = 0.01; w < 60; w += 0.01)
            omega.push_back(w);

        GraphWindow::addPoints(numChartH,
                               MathCore::calculateImpulseResponse(
                                   numeratorData,
                                   denominatorData,
                                   time),
                               "Тест");
        GraphWindow::addComplexPoints(numChartFreqResp,
                                      MathCore::calculateFrequencyResponse(
                                          numeratorData,
                                          denominatorData,
                                          omega),
                                      "Тест");
        /*setChart(numChartFreqResp,
                 "Комплексно-частотная характеристика (КЧХ)",
                 "Реальная ось", "Мнимая ось"
        );*/
    });

    numChartH = new QChart();
    numChartFreqResp = new QChart();

    setChart(numChartH,
             "Переходная характеристика",
             "Ось времени", "Ось значений"
    );
    setChart(numChartFreqResp,
             "Комплексно-частотная характеристика (КЧХ)",
             "Реальная ось", "Мнимая ось"
    );

    QWidget *numTab = new QWidget();

    numChartHView = new QChartView(numChartH, numTab);
    numChartFreqRespView = new QChartView(numChartFreqResp, numTab);

    QHBoxLayout* chartsLayout = new QHBoxLayout;
    chartsLayout->addWidget(numChartHView);
    chartsLayout->addWidget(numChartFreqRespView);

    QVBoxLayout *resLayout = new QVBoxLayout;
    resLayout->addLayout(uppLayout);
    resLayout->addWidget(calculateButton); // Добавляем кнопку в макет
    resLayout->addLayout(chartsLayout);

    numTab->setLayout(resLayout);
    return numTab;
}

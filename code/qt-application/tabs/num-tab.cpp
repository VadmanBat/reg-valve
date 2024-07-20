//
// Created by Vadma on 08.07.2024.
//
#include "../application.h"

QWidget* Application::createNumTab() {
    QLabel *transferFunctionLabel = new QLabel("W(p) = ");
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

        if (denominatorData.empty())
            return;
        if (numeratorData.size() > denominatorData.size())
            return;

        TransferFunction W(numeratorData, denominatorData);
        std::cout << "is settled: " << W.isSettled() << '\n';
        std::cout << "settling time: " << W.settlingTime() << '\n';
        std::cout << "steady state value: " << W.steadyStateValue() << '\n';

        removeAllSeries(numChartTranResp);
        removeAllSeries(numChartFreqResp);

        Application::addPoints(numChartTranResp, W.transientResponse(), "Тест");
        Application::addComplexPoints(numChartFreqResp, W.frequencyResponse(), "Тест");

        updateAxes(numChartTranResp);
        updateAxes(numChartFreqResp);
    });

    QWidget *numTab = new QWidget();
    auto chartsLayout = createCharts(NUM_CHARTS, numTab);

    QVBoxLayout *resLayout = new QVBoxLayout;
    resLayout->addLayout(uppLayout);
    resLayout->addWidget(calculateButton); // Добавляем кнопку в макет
    resLayout->addLayout(chartsLayout);

    numTab->setLayout(resLayout);
    return numTab;
}

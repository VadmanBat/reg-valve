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

        if (numeratorData.size() > denominatorData.size())
            return;

        auto time(MathCore::range(0, 120, 100));

        auto [a, b](RegCore::computeFrequencyRange(numeratorData, denominatorData, 0.01));

        auto freq(MathCore::logRange(a, b, 100, true));

        //auto impulseResponse = MathCore::talbotMethod(numeratorData, denominatorData, time);
        auto transietResponse = RegCore::calculateTransietResponse(numeratorData, denominatorData, time);
        auto frequencyResponse = RegCore::calculateFrequencyResponse(numeratorData, denominatorData, freq);

        /*for (auto x : frequencyResponse)
            qDebug() << std::abs(x) << '\n';*/

        removeAllSeries(numChartTranResp);
        removeAllSeries(numChartFreqResp);

        GraphWindow::addPoints(numChartTranResp, transietResponse, "Тест");
        GraphWindow::addComplexPoints(numChartFreqResp, frequencyResponse, "Тест");

        updateAxes(numChartTranResp);
        updateAxes(numChartFreqResp);
    });

    numChartTranResp = new QChart();
    numChartFreqResp = new QChart();

    numChartTranResp->setTitle("Переходная характеристика");
    numChartFreqResp->setTitle("Комплексно-частотная характеристика (КЧХ)");
    createAxes(numChartTranResp, "Ось времени", "Ось значений");
    createAxes(numChartFreqResp, "Реальная ось", "Мнимая ось");

    QWidget *numTab = new QWidget();

    numChartTranRespView = new QChartView(numChartTranResp, numTab);
    numChartFreqRespView = new QChartView(numChartFreqResp, numTab);

    QHBoxLayout* chartsLayout = new QHBoxLayout;
    chartsLayout->addWidget(numChartTranRespView);
    chartsLayout->addWidget(numChartFreqRespView);

    QVBoxLayout *resLayout = new QVBoxLayout;
    resLayout->addLayout(uppLayout);
    resLayout->addWidget(calculateButton); // Добавляем кнопку в макет
    resLayout->addLayout(chartsLayout);

    numTab->setLayout(resLayout);
    return numTab;
}

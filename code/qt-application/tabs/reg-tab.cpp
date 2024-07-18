//
// Created by Vadma on 18.07.2024.
//
#include "../application.h"

QWidget* GraphWindow::createRegTab() {
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

        TransferFunction W(numeratorData, denominatorData);
        W.closeLoop();
        std::cout << "is settled: " << W.isSettled() << '\n';
        std::cout << "settling time: " << W.settlingTime() << '\n';
        std::cout << "steady state value: " << W.steadyStateValue() << '\n';

        removeAllSeries(regChartTranResp);
        removeAllSeries(regChartFreqResp);

        GraphWindow::addPoints(regChartTranResp, W.transientResponse(), "Тест");
        GraphWindow::addComplexPoints(regChartFreqResp, W.frequencyResponse(), "Тест");

        updateAxes(regChartTranResp);
        updateAxes(regChartFreqResp);
    });

    regChartTranResp = new QChart();
    regChartFreqResp = new QChart();

    regChartTranResp->setTitle("Переходная характеристика");
    regChartFreqResp->setTitle("Комплексно-частотная характеристика (КЧХ)");
    createAxes(regChartTranResp, "Время t, секунды", "Параметр h(t), [ед.изм.] / %ХРО");
    createAxes(regChartFreqResp, "Реальная ось", "Мнимая ось");

    QWidget *numTab = new QWidget();

    regChartTranRespView = new QChartView(regChartTranResp, numTab);
    regChartFreqRespView = new QChartView(regChartFreqResp, numTab);

    QHBoxLayout* chartsLayout = new QHBoxLayout;
    chartsLayout->addWidget(regChartTranRespView);
    chartsLayout->addWidget(regChartFreqRespView);

    QVBoxLayout *resLayout = new QVBoxLayout;
    resLayout->addLayout(uppLayout);
    resLayout->addWidget(calculateButton); // Добавляем кнопку в макет
    resLayout->addLayout(chartsLayout);

    numTab->setLayout(resLayout);
    return numTab;
}

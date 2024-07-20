//
// Created by Vadma on 08.07.2024.
//
#include "../application.h"

QWidget* Application::createNumTab() {
    auto transferFunctionLayout = createTransferFunctionForm(numNumerator, numDenominator);

    auto calculateButton = new QPushButton("Рассчитать");
    connect(calculateButton, &QPushButton::clicked, [this] {
        auto numerator = getLineEditData(numNumerator);
        auto denominator = getLineEditData(numDenominator);

        if (denominator.empty())
            return;
        if (numerator.size() > denominator.size())
            return;

        TransferFunction W(numerator, denominator);
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

    auto numTab = new QWidget;
    auto chartsLayout = createCharts(NUM_CHARTS, numTab);

    auto layout = new QVBoxLayout;
    layout->addLayout(transferFunctionLayout);
    layout->addWidget(calculateButton);
    layout->addLayout(chartsLayout);

    numTab->setLayout(layout);
    return numTab;
}

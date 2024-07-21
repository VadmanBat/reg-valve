//
// Created by Vadma on 21.07.2024.
//
#include "../application.h"

void Application::numAddTransferFunction() {
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

    Application::addPoints(numChartTranResp, W.transientResponse(), "Тест");
    Application::addComplexPoints(numChartFreqResp, W.frequencyResponse(), "Тест");

    updateAxes(numChartTranResp);
    updateAxes(numChartFreqResp);
}

void Application::numReplaceTransferFunction() {
    eraseLastSeries(numChartTranResp);
    eraseLastSeries(numChartFreqResp);

    numAddTransferFunction();
}

void Application::numClearCharts() {
    removeAllSeries(numChartTranResp);
    removeAllSeries(numChartFreqResp);
}
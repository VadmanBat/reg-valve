//
// Created by Vadma on 21.07.2024.
//
#include "../application.h"

bool Application::numIsValidInput(const MathCore::Vec& num, const MathCore::Vec& den) {
    if (den.empty()) {
        showError("Знаменатель НЕ может быть равен нулю!");
        return false;
    }
    if (den.back() == 0) {
        showError("Свободный член знаменателя НЕ может быть равен нулю!");
        return false;
    }
    if (den.size() == 1) {
        showError("Порядок знаменателя НЕ может быть меньше первого порядка!");
        return false;
    }
    if (num.size() > den.size()) {
        showError("Порядок числителя НЕ может быть больше порядка знаменателя!");
        return false;
    }
    return true;
}

void Application::numAddTransferFunction() {
    auto numerator = getLineEditData(numNumerator);
    auto denominator = getLineEditData(numDenominator);

    if (numIsValidInput(numerator, denominator)) {
        TransferFunction W(numerator, denominator);
        std::cout << "is settled: " << W.isSettled() << '\n';
        std::cout << "settling time: " << W.settlingTime() << '\n';
        std::cout << "steady state value: " << W.steadyStateValue() << '\n';

        Application::addPoints(numChartTranResp, W.transientResponse(), "Тест");
        Application::addComplexPoints(numChartFreqResp, W.frequencyResponse(), "Тест");

        updateAxes(numChartTranResp);
        updateAxes(numChartFreqResp);
    }
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
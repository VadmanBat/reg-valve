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

        numTranRespSeries.push_back(Series(W.transientResponse()));
        numFreqRespSeries.push_back(W.frequencyResponse());

        Application::addPoints(numChartTranResp, numTranRespSeries.back().original(), "Тест");
        Application::addComplexPoints(numChartFreqResp, numFreqRespSeries.back().original(), "Тест");

        updateAxes(
                regChartTranResp,
                {numTranRespSeries.min_x(), numTranRespSeries.max_x()},
                computeAxesRange(numTranRespSeries.min_y(), numTranRespSeries.max_y())
        );
        updateAxes(
                regChartFreqResp,
                computeAxesRange(numFreqRespSeries.min_x(), numFreqRespSeries.max_x()),
                computeAxesRange(numFreqRespSeries.min_y(), numFreqRespSeries.max_y())
        );
    }
}

void Application::numReplaceTransferFunction() {
    if (!numTranRespSeries.empty())
        numTranRespSeries.pop_back();
    eraseLastSeries(numChartTranResp);
    eraseLastSeries(numChartFreqResp);

    numAddTransferFunction();
}

void Application::numClearCharts() {
    removeAllSeries(numChartTranResp);
    removeAllSeries(numChartFreqResp);
}
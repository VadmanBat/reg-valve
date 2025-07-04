//
// Created by Vadim on 21.07.2024.
//
#include "../application.h"

bool Application::numIsValidInput(const MathCore::Vec& num, const MathCore::Vec& den) {
    if (den.empty()) {
        showError("Знаменатель НЕ может быть равен нулю!");
        return false;
    }
    /*if (den.back() == 0) {
        showError("Свободный член знаменателя НЕ может быть равен нулю!");
        return false;
    }*/
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
    auto numerator = numTF.getNum();
    auto denominator = numTF.getDen();

    if (numIsValidInput(numerator, denominator)) {
        //TransferFunction W(numerator, denominator);
#define W numTranFunc
        W = getTransferFunction(numTF);
        numTranRespSeries.push_back(getTranResp(W, numModelParam));
        numFreqRespSeries.push_back(getFreqResp(W, numModelParam));

        const auto& tranResp = numTranRespSeries.back().original();
        const auto& freqResp = numFreqRespSeries.back().original();

        numWidget->updateValues(W.isSettled() ? std::vector <double>{
                W.settlingTime(),   W.naturalFrequency(),
                W.riseTime(),       W.cutFrequency(),
                W.dampingRation(),  W.steadyStateValue()
        } : std::vector <double>{});
#undef W //numTranFunc

        Application::addPoints(numChartTranResp, tranResp, "Тест", numSize);
        Application::addComplexPoints(numChartFreqResp, freqResp, "Тест", numSize);

        ++numSize;

        updateAxes(
                numChartTranResp,
                {numTranRespSeries.min_x(), numTranRespSeries.max_x()},
                computeAxesRange(numTranRespSeries.min_y(), numTranRespSeries.max_y())
        );
        updateAxes(
                numChartFreqResp,
                computeAxesRange(numFreqRespSeries.min_x(), numFreqRespSeries.max_x()),
                computeAxesRange(numFreqRespSeries.min_y(), numFreqRespSeries.max_y())
        );
    }
}

void Application::numReplaceTransferFunction() {
    if (!numTranRespSeries.empty()) {
        eraseLastSeries(numChartTranResp);
        numTranRespSeries.pop_back();
        --numSize;
    }
    if (!numFreqRespSeries.empty()) {
        eraseLastSeries(numChartFreqResp);
        numFreqRespSeries.pop_back();
    }

    numAddTransferFunction();
}

void Application::numClearCharts() {
    removeAllSeries(numChartTranResp);
    removeAllSeries(numChartFreqResp);

    numTranRespSeries.clear();
    numFreqRespSeries.clear();

    numSize = 0;
}
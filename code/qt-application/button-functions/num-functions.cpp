//
// Created by Vadma on 21.07.2024.
//
#include "../application.h"
#include "../structures/dialogs/mod-par-dialog.hpp"

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

Application::VecPair Application::getNumTranResp(const TransferFunction& W, const ModelParam& params) {
    if (params.autoSimTime)
        return W.transientResponse();
    if (params.autoTimeIntervals)
        return W.transientResponse({0, params.simTime});
    return W.transientResponse({0, params.simTime}, params.timeIntervals);
}
/*
Application::VecComp Application::getFreqTranResp(const TransferFunction& W, const ModelParam& params) {
    if (params.autoFreqRange)
        return W.frequencyResponse();
    return W.transientResponse({0, params.simTime}, params.autoTimeIntervals ? 200 : params.timeIntervals);
}*/

void Application::numAddTransferFunction() {
    auto numerator = getLineEditData(numNumerator);
    auto denominator = getLineEditData(numDenominator);

    if (numIsValidInput(numerator, denominator)) {
        TransferFunction W(numerator, denominator);
        numTranRespSeries.push_back(getNumTranResp(W, numModelParam));
        numFreqRespSeries.push_back(W.frequencyResponse());

        const auto& tranResp = numTranRespSeries.back().original();
        const auto& freqResp = numFreqRespSeries.back().original();

        numWidget->updateValues(W.isSettled() ? std::vector <double>{
                W.settlingTime(), W.naturalFrequency(),
                W.riseTime(), W.cutFrequency(),
                W.dampingRation(), W.steadyStateValue()
        } : std::vector <double>{});

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

void Application::numShowModParDialog() {
    ModParDialog dialog(numModelParam);
    if (dialog.exec() == QDialog::Accepted) {
        numModelParam = dialog.data();
    }
}
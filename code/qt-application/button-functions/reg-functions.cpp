//
// Created by Vadma on 21.07.2024.
//
#include "../application.h"

void Application::regAddTransferFunction() {
    auto numerator = getLineEditData(regNumerator);
    auto denominator = getLineEditData(regDenominator);

    if (denominator.empty())
        return;
    if (numerator.size() > denominator.size())
        return;

    std::vector <double> num, den;

    double Kp = regParameters[0]->getSlider()->value();
    double Tu = regParameters[1]->getSlider()->value();
    double Td = regParameters[2]->getSlider()->value();
    bool f1(regParameters[0]->getSlider()->isEnabled());
    bool f2(regParameters[1]->getSlider()->isEnabled());
    bool f3(regParameters[2]->getSlider()->isEnabled());

    if (!f1)
        Kp = 1;

    switch (f3 * 2 + f2) {
        case 0:
            num = {Kp};
            den = {1};
            break;
        case 1:
            num = {Kp * Tu, Kp};
            den = {Tu, 0};
            break;
        case 2:
            num = {Kp * Td, Kp};
            den = {1};
            break;
        case 3:
            num = {Kp * Tu * Td, Kp * Tu, Kp};
            den = {Tu, 0};
            break;
    }

    TransferFunction W(numerator, denominator, num, den);

    regWidget->updateValues(W.isSettled() ? std::vector <double>{
            W.settlingTime(), W.naturalFrequency(), W.steadyStateValue(),  W.computeLinearIntegralCriterion(),
            W.riseTime(), W.cutFrequency(), 1 - W.steadyStateValue(), W.computeIntegralQuadraticCriterion(),
            W.peakTime(), W.dampingRation(), W.overShoot(), W.computeStandardDeviation()
    } : std::vector <double>{});

    std::stringstream stream;
    stream << "(" << Kp << ", " << Tu << ", " << Td << ")";
    auto string (stream.str());

    Application::addPoints(regChartTranResp, W.isSettled() ? W.transientResponse() : W.transientResponse({0, 1000}), string.c_str());
    Application::addComplexPoints(regChartFreqResp, W.frequencyResponse(), string.c_str());

    updateAxes(regChartTranResp);
    updateAxes(regChartFreqResp);
}

void Application::regReplaceTransferFunction() {
    eraseLastSeries(regChartTranResp);
    eraseLastSeries(regChartFreqResp);

    regAddTransferFunction();
}

void Application::regClearCharts() {
    removeAllSeries(regChartTranResp);
    removeAllSeries(regChartFreqResp);
}
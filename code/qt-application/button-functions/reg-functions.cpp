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

    switch (4 * f3 + 2 * f2 + f1) {
        case 0: /// 1
            num = {1};
            den = {1};
            break;
        case 1: /// P: Kp
            num = {Kp};
            den = {1};
            break;
        case 2: /// I: 1 / (Tu * p)
            num = {1};
            den = {Tu, 0};
            break;
        case 3: /// PI: (Kp * p + Kp / Tu) / p
            num = {Kp, Kp / Tu};
            den = {1, 0};
            break;
        case 4: /// D: Td * p
            num = {Td, 0};
            den = {1};
            break;
        case 5: /// PD: Kp * Td * p + Kp
            num = {Kp * Td, Kp};
            den = {1};
            break;
        case 6: /// ID: (Td * p^2 + 1 / Tu) / (p)
            num = {Td, 0, 1 / Tu};
            den = {1, 0};
            break;
        case 7: /// PID: (Kp * Td * p^2 + Kp * p + kp / Tu) / p
            num = {Kp * Td, Kp, Kp / Tu};
            den = {1, 0};
            break;
    }

    TransferFunction W(numerator, denominator, num, den);

    regWidget->updateValues(W.isSettled() ? std::vector <double>{
            W.settlingTime(), W.naturalFrequency(), W.steadyStateValue(),  W.computeLinearIntegralCriterion(),
            W.riseTime(), W.cutFrequency(), 1 - W.steadyStateValue(), W.computeIntegralQuadraticCriterion(),
            W.peakTime(), W.dampingRation(), W.overshoot(), W.computeStandardDeviation()
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
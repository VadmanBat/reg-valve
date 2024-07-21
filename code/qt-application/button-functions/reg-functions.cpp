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

    double Kp = regSliders[0]->value();
    double Tu = regSliders[1]->value();
    double Td = regSliders[2]->value();

    bool f1(regSliders[0]->isEnabled()), f2(regSliders[1]->isEnabled()), f3(regSliders[2]->isEnabled());

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

    std::cout << "is settled: " << W.isSettled() << '\n';
    std::cout << "settling time: " << W.settlingTime() << '\n';
    std::cout << "steady state value: " << W.steadyStateValue() << '\n';

    std::cout << "Kp = " << regSliders[0]->value() << '\n';
    std::cout << "Tu = " << regSliders[1]->value() << '\n';
    std::cout << "Td = " << regSliders[2]->value() << '\n';

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
//
// Created by Vadim on 21.07.2024.
//
#include "../application.h"

void Application::regAddTransferFunction() {
    auto numerator = regTF.getNum();
    auto denominator = regTF.getDen();

    if (denominator.empty())
        return;
    /*if (numerator.size() > denominator.size())
        return;*/

    std::vector <double> num, den;

    const double Kp = regParameters[0]->getSlider()->value();
    const double Tu = regParameters[1]->getSlider()->value();
    const double Td = regParameters[2]->getSlider()->value();
    const bool P = regParameters[0]->getSlider()->isEnabled();
    const bool I = regParameters[1]->getSlider()->isEnabled();
    const bool D = regParameters[2]->getSlider()->isEnabled();
    const int reg_id = P + 2 * I + 4 * D;
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2);

    switch (reg_id) {
        case 0: /// 1
            num = {1};
            den = {1};
            stream << "1";
            break;
        case 1: /// P: Kp
            num = {Kp};
            den = {1};
            stream << "П(" << Kp << ")";
            break;
        case 2: /// I: 1 / (Tu * p)
            num = {1};
            den = {Tu, 0};
            stream << "И(" << Tu << ")";
            break;
        case 3: /// PI: (Kp * p + Kp / Tu) / p
            num = {Kp, Kp / Tu};
            den = {1, 0};
            stream << "ПИ(" << Kp << ", " << Tu << ")";
            break;
        case 4: /// D: Td * p
            num = {Td, 0};
            den = {1};
            stream << "Д(" << Td << ")";
            break;
        case 5: /// PD: Kp * Td * p + Kp
            num = {Kp * Td, Kp};
            den = {1};
            stream << "ПД(" << Kp << ", " << Td << ")";
            break;
        case 6: /// ID: (Td * p^2 + 1 / Tu) / (p)
            num = {Td, 0, 1 / Tu};
            den = {1, 0};
            stream << "ИД(" << Tu << ", " << Td << ")";
            break;
        case 7: /// PID: (Kp * Td * p^2 + Kp * p + kp / Tu) / p
            num = {Kp * Td, Kp, Kp / Tu};
            den = {1, 0};
            stream << "ПИД(" << Kp << ", " << Tu << ", " << Td << ")";
            break;
    }

    //TransferFunction W(numerator, denominator, num, den, tau, order);
#define W regTranFunc
    W = TransferFunction(getRegTransferFunction(regTF, num, den));
    regTranRespSeries.push_back(getTranResp(W, regModelParam));
    regFreqRespSeries.push_back(getFreqResp(W, regModelParam));std::cout << "Hello" << '\n';

    const auto& tranResp = regTranRespSeries.back().original();
    const auto& freqResp = regFreqRespSeries.back().original();

    regWidget->updateValues(W.isSettled() ? std::vector <double>{
            W.settlingTime(),   W.naturalFrequency(),   W.steadyStateValue(),       W.computeLinearIntegralCriterion(tranResp),
            W.riseTime(),       W.cutFrequency(),       1 - W.steadyStateValue(),   W.computeIntegralQuadraticCriterion(),
            W.peakTime(),       W.dampingRation(),      W.overshoot(),              W.computeStandardDeviation()
    } : std::vector <double>{});
#undef W //regTranFunc

    const auto title  = stream.str();
    Application::addPoints(regChartTranResp, tranResp, title.c_str(), regSize);
    Application::addComplexPoints(regChartFreqResp, freqResp, title.c_str(), regSize);

    ++regSize;

    updateAxes(
            regChartTranResp,
            {regTranRespSeries.min_x(), regTranRespSeries.max_x()},
            computeAxesRange(regTranRespSeries.min_y(), regTranRespSeries.max_y())
    );
    updateAxes(
            regChartFreqResp,
            computeAxesRange(regFreqRespSeries.min_x(), regFreqRespSeries.max_x()),
            computeAxesRange(regFreqRespSeries.min_y(), regFreqRespSeries.max_y())
    );
}

void Application::regReplaceTransferFunction() {
    if (!regTranRespSeries.empty()) {
        eraseLastSeries(regChartTranResp);
        regTranRespSeries.pop_back();
        --regSize;
    }
    if (!regFreqRespSeries.empty()) {
        eraseLastSeries(regChartFreqResp);
        regFreqRespSeries.pop_back();
    }

    regAddTransferFunction();
}

void Application::regClearCharts() {
    removeAllSeries(regChartTranResp);
    removeAllSeries(regChartFreqResp);
    regTranRespSeries.clear();
    regFreqRespSeries.clear();

    regSize = 0;
}
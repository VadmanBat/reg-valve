//
// Created by Vadma on 26.12.2024.
//
#include "../application.h"
#include "code/structures/dialogs/mod-par-dialog.hpp"

void Application::showModParDialog(ModelParam& params) {
    ModParDialog dialog(params);
    if (dialog.exec() == QDialog::Accepted)
        params = dialog.data();
}

Application::VecPair Application::getTranResp(const TransferFunction& W, const ModelParam& params) {
    if (params.autoSimTime)
        return W.transientResponse();
    if (params.autoTimeIntervals)
        return W.transientResponse({0, params.simTime});
    return W.transientResponse({0, params.simTime}, (std::size_t)params.timeIntervals);
}

Application::VecComp Application::getFreqResp(const TransferFunction& W, const ModelParam& params) {
    if (params.autoFreqRange)
        return W.frequencyResponse();
    if (params.autoFreqIntervals)
        return W.frequencyResponse({params.freqMin, params.freqMax}, 1e-2);
    if (params.freqScale == 1)
        return W.frequencyResponse({params.freqMin, params.freqMax}, params.freqIntervals, true);
    return W.frequencyResponse({params.freqMin, params.freqMax}, (std::size_t)params.freqIntervals);
}

TransferFunction Application::getTransferFunction(const TransferFunctionForm& form) {
    auto n = form.getNum();
    if (form.isDelayElement()) {
        return {n, form.getDen(), form.delayTime(), 6};
    }
    return {n, form.getDen()};
}

TransferFunction Application::getRegTransferFunction(const TransferFunctionForm& form, std::vector <double> a, std::vector <double> b) {
    auto n = form.getNum();
    if (form.isDelayElement())
        return {n, form.getDen(), a, b, form.delayTime(), 6};
    return {n, form.getDen(), a, b};
}
//
// Created by Vadim on 21.07.2024.
//
#include "../application.h"

#include "../../reg-core.hpp"

void Application::expOpenFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Файлы данных (*.txt *.csv)");
    if (fileName.isEmpty())
        return;

    if (!expTranRespSeries.empty()) {
        eraseLastSeries(expChartTranResp);
        expTranRespSeries.pop_back();
    }

    auto tranData = readVectorFromFile(fileName);
    //auto freqResp = RegCore::calculateFrequencyResponse(tranData, 100);

    expTranRespSeries.push_back(tranData);
    //expFreqRespSeries.push_back(freqResp);

    Application::addPoints(expChartTranResp, tranData, "Тест");
    //Application::addComplexPoints(expChartFreqResp, freqResp, "Тест", numSize);

    updateAxes(
            expChartTranResp,
            {expTranRespSeries.min_x(), expTranRespSeries.max_x()},
            computeAxesRange(expTranRespSeries.min_y(), expTranRespSeries.max_y())
    );
    /*updateAxes(
            expChartFreqResp,
            computeAxesRange(expFreqRespSeries.min_x(), expFreqRespSeries.max_x()),
            computeAxesRange(expFreqRespSeries.min_y(), expFreqRespSeries.max_y())
    );*/
}

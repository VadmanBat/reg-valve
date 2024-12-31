//
// Created by Vadma on 15.12.2024.
//

#ifndef REGVALVE_MODEL_PARAM_HPP
#define REGVALVE_MODEL_PARAM_HPP

struct ModelParam {
    bool autoSimTime        = true;
    bool autoTimeIntervals  = true;
    bool autoFreqRange      = true;
    bool autoFreqIntervals  = true;
    bool hybridDelayElement = true;

    int simTime         = 500;
    int timeIntervals   = 100;
    int freqIntervals   = 100;
    int approxOrder     = 3;

    double freqMin = 0.01;
    double freqMax = 10;

    int freqScale = 0;
};

#endif //REGVALVE_MODEL_PARAM_HPP

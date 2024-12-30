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

    int simTime         = 500;
    std::size_t timeIntervals   = 100;
    std::size_t freqIntervals   = 100;

    double freqMin = 0.01;
    double freqMax = 10;

    int freqScale = 0;
};

#endif //REGVALVE_MODEL_PARAM_HPP

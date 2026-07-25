#pragma once

struct ModelParam {
    bool autoSimTime       = true;
    bool autoTimeIntervals = true;
    bool autoFreqRange     = true;
    bool autoFreqIntervals = true;

    int simTime       = 500;
    int timeIntervals = 100;
    int freqIntervals = 100;
    int approxOrder   = 3;

    double freqMin = 0.01;
    double freqMax = 10.0;

    int freqScale = 0; // 0 = log, 1 = linear
};

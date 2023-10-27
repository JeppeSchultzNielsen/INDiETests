//
// Created by jeppe on 7/26/23.
//

#ifndef TRACEANALYSIS_WRITETIME_HH
#define TRACEANALYSIS_WRITETIME_HH

#include "TString.h"
#include <ctime>
#include <iostream>

using namespace std;

class TimeWriter {
public:
    double start_clock;
    int printEvery;

    void start(){
        start_clock = clock();
    }

    TimeWriter(int printEvery){
        this->printEvery = printEvery;
    }

    void writeTime(int current_block, int full_size);
};

#endif //TRACEANALYSIS_WRITETIME_HH

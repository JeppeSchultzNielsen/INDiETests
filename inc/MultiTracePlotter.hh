//
// Created by jeppe on 11/12/23.
//

#ifndef INDIETESTS_MULTITRACEPLOTTER_HH
#define INDIETESTS_MULTITRACEPLOTTER_HH

#include "TString.h"
#include "TGraph.h"
#include "TFile.h"
#include "vector"
#include "TracePlotter.hh"
#include "TDirectory.h"

using namespace std;

class MultiTracePlotter {
public:
    TString inputNameCycle;
    vector<int> runNumbers;
    vector<double> positions;
    vector<TracePlotter*> tracePlotters;

    MultiTracePlotter(vector<int> runNumbers, vector<double> positions, TString inputNameCycle){
        this-> runNumbers = runNumbers;
        this-> positions = positions;
        this-> inputNameCycle = inputNameCycle;
    }

    void doAnalyses();

    void saveHistograms(TString fileName);

    void doAnalysesSingle();

    void saveHistogramsSingle(TString fileName);
};

#endif //INDIETESTS_MULTITRACEPLOTTER_HH

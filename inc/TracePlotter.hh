//
// Created by jeppe on 11/12/23.
//

#ifndef INDIETESTS_TRACEPLOTTER_HH
#define INDIETESTS_TRACEPLOTTER_HH

#include "TString.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TH1D.h"
#include "vector"
#include "TGraphErrors.h"
#include "iostream"
#include "timeWriter.hh"
#include "dig_daq_param.hh"
#include "Trace.hh"

using namespace std;

class TracePlotter {
public:
    TGraph *avgTraceGraph;
    TH1D* avgTraceL;
    TH1D* avgTraceR;
    TH1D* stdTraceL;
    TH1D* stdTraceR;
    TH1D* frontBackRatiosL;
    TH1D* frontBackRatiosR;
    TGraphErrors *traceLGraph;
    TGraphErrors *traceRGraph;
    TMultiGraph *tracesL;
    TMultiGraph *tracesR;

    TFile *file;
    double position;

    TracePlotter(double position){
        this -> position = position;
    }

    void loopOverFile(TString fileName);

    void createHistograms();

    void loopOverFileSingle(TString fileName);

    void createHistogramSingle();
};

#endif //INDIETESTS_TRACEPLOTTER_HH

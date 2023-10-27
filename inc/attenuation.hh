//
// Created by jeppe on 10/23/23.
//

#ifndef NEUTRONTOF_ATTENUATION_HH
#define NEUTRONTOF_ATTENUATION_HH

#include "TString.h"
#include "TFile.h"
#include "iostream"
#include "TH2D.h"
#include "TH1D.h"
#include "TTree.h"
#include "gaussFitter.h"
#include "TGraph.h"
#include "TF1.h"

using namespace std;

class AttenuationAnalysis{
public:
    int runNum;
    double position;
    TH2D *attenuationHist;
    vector<TH1D *> attenuationProjections;
    vector<double> e_r;
    vector<double> e_l;
    TGraph *attenuationGraph;
    bool projectionsFitted = false;
    double comptonPeak;

    AttenuationAnalysis(int runNum, double position){
        this -> runNum = runNum;
        this -> position = position;
    }

    void loopOverFile(TString fileName);

    void createProjections();

    void fitProjections();

    void saveHistograms(TString saveTo);

    double findComptonPeak();

    void loadHistograms();

    pair<double,double> fitScale();

};

#endif //NEUTRONTOF_ATTENUATION_HH

//
// Created by jeppe on 10/27/23.
//

#ifndef INDIETESTS_WALKCORRECTOR_H
#define INDIETESTS_WALKCORRECTOR_H

#include "TString.h"
#include "TFile.h"
#include "iostream"
#include "TH2D.h"
#include "TH1D.h"
#include "TTree.h"
#include "gaussFitter.h"
#include "TGraph.h"
#include "TF1.h"
#include "timeWriter.hh"

using namespace std;

class WalkCorrector{
public:
    TH2D *walkHist;
    TH2D *QDCvsQDC;
    vector<TH1D *> walkTH1s;
    vector<pair<double,double>> lowerEdges;
    vector<double> walkMeans;
    vector<double> walkSigmas;

    double dR;
    double dL;

    WalkCorrector(){}

    void loopOverFile(TString fileName);

    void saveHistograms(TString saveTo);

    void fitWalkTH1s();
};

#endif //INDIETESTS_WALKCORRECTOR_H

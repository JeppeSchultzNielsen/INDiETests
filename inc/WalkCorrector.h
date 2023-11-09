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
#include "gsl/gsl_multimin.h"
#include "fstream"

using namespace std;

class WalkCorrector{
public:
    TH2D *walkHist;
    TH2D *QDCvsQDC;
    vector<TH1D *> walkTH1s;
    vector<pair<double,double>> lowerEdges;
    vector<double> walkMeans;
    vector<double> walkSigmas;
    vector<double> centersL;
    vector<double> centersR;
    double loadedPar[3];
    vector<double> walkPar;
    bool loadedWalkPars = false;

    double dR;
    double dL;

    WalkCorrector(){}

    void loopOverFile(TString fileName);

    void saveHistograms(TString saveTo);

    void fitWalkTH1s();

    static double getWalkCorrection(double QDC, double *par);

    void loadVectors(TString fileName);

    void saveVectors(TString saveTo);

    double getWalkCorrection(double QDC);

    static double toMinimize(const gsl_vector *v, void *params);

    void doMinimization();

    void writeWalkPars(TString saveTo);

    void readWalkPars(TString fileName);


};

#endif //INDIETESTS_WALKCORRECTOR_H

//
// Created by jeppe on 11/8/23.
//

#ifndef INDIETESTS_VELOCITYMEASUREMENT_H
#define INDIETESTS_VELOCITYMEASUREMENT_H

#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TMath.h>
#include <TF1.h>
#include <TLine.h>
#include <TGraph2D.h>
#include <TH1D.h>
#include "WalkCorrector.h"
#include "gaussFitter.h"
#include "Positioner.hh"

class VelocityMeasurement {
public:
    double position;
    int runNumber;
    TH1D* correctedSpectrum;
    TH1D* correctedSumSpectrum;
    TH1D* uncorrectedSpectrum;
    TH1D* posSpectrumLin;
    TH1D* posSpectrumPoly2;
    TH1D* posSpectrumPoly2ThenFlat;
    GaussFitter* correctedGaussFitter;
    GaussFitter* uncorrectedGaussFitter;
    GaussFitter* posGaussFitterLin;
    GaussFitter* posGaussFitterPoly2;
    GaussFitter* posGaussFitterPoly2ThenFlat;
    double t;

    VelocityMeasurement(int runNumber, double position){
        this-> runNumber = runNumber;
        this-> position = position;
    }

    void createHistograms(TString fileName);

    void fitHistograms();

    void createPositionHistograms(TString fileName);

    void fitPositionHistograms();
};
#endif //INDIETESTS_VELOCITYMEASUREMENT_H

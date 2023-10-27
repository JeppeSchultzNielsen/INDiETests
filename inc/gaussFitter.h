//
// Created by jeppe on 10/25/23.
//

#ifndef NEUTRONTOF_GAUSSFITTER_H
#define NEUTRONTOF_GAUSSFITTER_H

#include "TH1D.h"
#include "TMath.h"
#include "TF1.h"

class GaussFitter{
public:
    TH1D *histToFit;
    double maxVal;
    int maxBin;
    double maxPos;
    int leftHalfMaxBin;
    int rightHalfMaxBin;
    double leftHalfMax;
    double rightHalfMax;
    double FWHM;
    double offset;
    double slope;

    double fitAmplitude;
    double fitMean;
    double fitSigma;
    double fitOffset;
    double fitSlope;

    double fitAmplitudeErr;
    double fitMeanErr;
    double fitSigmaErr;
    double fitOffsetErr;
    double fitSlopeErr;

    GaussFitter(TH1D *histToFit){
        this -> histToFit = histToFit;
        extractParameters();
    };

    static double gaussianLin(double *x, double *par);

    static double gaussian(double *x, double *par);

    void extractParameters();

    void fitGaussian(double fitStart, double fitEnd);

    void fitGaussianLin(double fitStart, double fitEnd);
};

#endif //NEUTRONTOF_GAUSSFITTER_H

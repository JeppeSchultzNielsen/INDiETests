//
// Created by jeppe on 10/25/23.
//

#include "gaussFitter.h"

double GaussFitter::gaussian(double *x, double *par) {
    double xx = x[0];
    double A = par[0];
    double mu = par[1];
    double sigma = par[2];
    double y = A/(sqrt(2.0 * TMath::Pi()) *sigma)  * exp(-0.5 * pow((xx - mu) / sigma, 2));
    return y;
}

double GaussFitter::gaussianLin(double *x, double *par) {
    double xx = x[0];
    double A = par[0];
    double mu = par[1];
    double sigma = par[2];
    double y = A/(sqrt(2.0 * TMath::Pi()) *sigma)  * exp(-0.5 * pow((xx - mu) / sigma, 2)) + par[3] * xx + par[4];
    return y;
}

void GaussFitter::extractParameters() {
    maxVal = histToFit->GetMaximum();
    maxBin = histToFit->GetMaximumBin();
    maxPos = histToFit->GetBinCenter(maxBin);
    leftHalfMaxBin = histToFit->FindFirstBinAbove(maxVal/2);
    rightHalfMaxBin = histToFit->FindLastBinAbove(maxVal/2);
    leftHalfMax = histToFit->GetBinCenter(leftHalfMaxBin);
    rightHalfMax = histToFit->GetBinCenter(rightHalfMaxBin);
    FWHM = rightHalfMax - leftHalfMax;
    offset = histToFit->GetBinContent(1);
    slope = 0;
}

void GaussFitter::fitGaussian(double fitStart, double fitEnd) {
    TF1 *gauss = new TF1("gauss", gaussian, fitStart, fitEnd, 3);
    gauss ->SetNpx(1000);
    gauss->SetParameter(0, maxVal);
    gauss->SetParameter(1, maxPos);
    gauss->SetParameter(2, FWHM/2.355);
    histToFit->Fit(gauss, "RQ","",fitStart,fitEnd);
    fitAmplitude = gauss->GetParameter(0);
    fitMean = gauss->GetParameter(1);
    fitSigma = gauss->GetParameter(2);
    fitAmplitudeErr = gauss->GetParError(0);
    fitMeanErr = gauss->GetParError(1);
    fitSigmaErr = gauss->GetParError(2);
}

void GaussFitter::fitGaussianLin(double fitStart, double fitEnd) {
    TF1 *gaussLin = new TF1("gaussLin", gaussianLin, fitStart, fitEnd, 5);
    gaussLin ->SetNpx(1000);
    gaussLin->SetParameter(0, maxVal);
    gaussLin->SetParameter(1, maxPos);
    gaussLin->SetParameter(2, FWHM/2.355);
    gaussLin->SetParameter(3, slope);
    gaussLin->SetParameter(4, offset);
    histToFit->Fit(gaussLin, "RQ","",fitStart,fitEnd);
    fitAmplitude = gaussLin->GetParameter(0);
    fitMean = gaussLin->GetParameter(1);
    fitSigma = gaussLin->GetParameter(2);
    fitOffset = gaussLin->GetParameter(4);
    fitSlope = gaussLin->GetParameter(3);
    fitAmplitudeErr = gaussLin->GetParError(0);
    fitMeanErr = gaussLin->GetParError(1);
    fitSigmaErr = gaussLin->GetParError(2);
    fitOffsetErr = gaussLin->GetParError(4);
    fitSlopeErr = gaussLin->GetParError(3);

}
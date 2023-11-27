//
// Created by jeppe on 11/8/23.
//

#include "VelocityMeasurement.h"

void VelocityMeasurement::createHistograms(TString fileName) {
    TFile* file = TFile::Open(fileName,"READ");
    TTree* tree = static_cast<TTree*>(file->Get("ids"));

    double en_INDiE[2];
    double t_INDiE[2];

    tree->SetBranchAddress("TraceIntegral_INDiE", en_INDiE);
    tree->SetBranchAddress("Time_INDiE", t_INDiE);

    correctedSpectrum = new TH1D(Form("correctedSpectrum %i",runNumber), Form("correctedSpectrum %i",runNumber), 500, -20, 20);
    correctedSumSpectrum = new TH1D(Form("correctedSumSpectrum %i",runNumber), Form("correctedSumSpectrum %i",runNumber), 500, -20, 20);
    uncorrectedSpectrum = new TH1D(Form("uncorrectedSpectrum %i",runNumber), Form("uncorrectedSpectrum %i",runNumber), 500, -20, 20);
    posSpectrumLin = new TH1D(Form("posSpectrumLin %i",runNumber), Form("posSpectrumLin %i",runNumber), 500, -100, 100);
    posSpectrumPoly2 = new TH1D(Form("posSpectrumPoly2 %i",runNumber), Form("posSpectrumPoly2 %i",runNumber), 500, -100, 100);
    posSpectrumPoly2ThenFlat = new TH1D(Form("posSpectrumPoly2ThenFlat %i",runNumber), Form("posSpectrumPoly2ThenFlat %i",runNumber), 500, -100, 100);

    Positioner *linPos = new Positioner();
    Positioner *poly2Pos = new Positioner();
    Positioner *poly2ThenFlatPos = new Positioner();
    linPos->loadParams("output/positionParamsLin.txt");
    poly2Pos->loadParams("output/positionParamsPoly2.txt");
    poly2ThenFlatPos->loadParams("output/positionParamsPoly2ThenFlat.txt");

    auto wc = new WalkCorrector();
    wc ->readWalkPars("output/walkCorrecterPars.txt");

    //read the timeoffset on the line corresponding to this runnumber
    ifstream file2;
    file2.open("output/timeOffsets.txt");
    double timeOffset;
    //read the runnumber'th line
    for (int i = 0; i < runNumber-1; ++i) {
        file2 >> timeOffset;
    }


    int nEntries = tree->GetEntries();
    for(int i = 0; i < nEntries; i++){
        tree->GetEntry(i);
        t_INDiE[0] = t_INDiE[0]*4.0;
        t_INDiE[1] = t_INDiE[1]*4.0;
        if(en_INDiE[0] > 0 && en_INDiE[1] > 0){
            double corrTimeDiff = t_INDiE[0] + wc->getWalkCorrection(en_INDiE[0]) - t_INDiE[1] - wc->getWalkCorrection(en_INDiE[1]);
            double corrTimeSum = t_INDiE[0] + wc->getWalkCorrection(en_INDiE[0]) + t_INDiE[1] + wc->getWalkCorrection(en_INDiE[1]);
            correctedSpectrum -> Fill(corrTimeDiff);
            uncorrectedSpectrum -> Fill(t_INDiE[0] - t_INDiE[1]);
            correctedSumSpectrum -> Fill(corrTimeSum - timeOffset);
            corrTimeDiff = corrTimeDiff - timeOffset;
        }
    }
}

void VelocityMeasurement::createPositionHistograms(TString fileName) {
    TFile* file = TFile::Open(fileName,"READ");
    TTree* tree = static_cast<TTree*>(file->Get("ids"));

    double en_INDiE[2];
    double t_INDiE[2];

    tree->SetBranchAddress("TraceIntegral_INDiE", en_INDiE);
    tree->SetBranchAddress("Time_INDiE", t_INDiE);

    posSpectrumLin = new TH1D(Form("posSpectrumLin %i",runNumber), Form("posSpectrumLin %i",runNumber), 500, -100, 100);
    posSpectrumPoly2 = new TH1D(Form("posSpectrumPoly2 %i",runNumber), Form("posSpectrumPoly2 %i",runNumber), 500, -100, 100);
    posSpectrumPoly2ThenFlat = new TH1D(Form("posSpectrumPoly2ThenFlat %i",runNumber), Form("posSpectrumPoly2ThenFlat %i",runNumber), 500, -100, 100);
    correctedSumSpectrum = new TH1D(Form("correctedSumSpectrum %i",runNumber), Form("correctedSumSpectrum %i",runNumber), 500, -40, 40);

    Positioner *linPos = new Positioner();
    Positioner *poly2Pos = new Positioner();
    Positioner *poly2ThenFlatPos = new Positioner();
    linPos->loadParams("output/positionParamsLin.txt");
    poly2Pos->loadParams("output/speedOfLightParams.txt");
    poly2ThenFlatPos->loadParams("output/positionParamsPoly2ThenFlat.txt");

    auto wc = new WalkCorrector();
    wc ->readWalkPars("output/walkCorrecterPars.txt");

    //read the timeoffset on the line corresponding to this runnumber
    ifstream file2;
    file2.open("output/timeOffsets.txt");
    double timeOffset;
    //read the runnumber'th line
    for (int i = 0; i < runNumber-1; ++i) {
        file2 >> timeOffset;
    }


    int nEntries = tree->GetEntries();
    for(int i = 0; i < nEntries; i++){
        tree->GetEntry(i);
        t_INDiE[0] = t_INDiE[0]*4.0;
        t_INDiE[1] = t_INDiE[1]*4.0;
        if(en_INDiE[0] > 0 && en_INDiE[1] > 0){
            double corrTimeDiff = t_INDiE[0] + wc->getWalkCorrection(en_INDiE[0]) - t_INDiE[1] - wc->getWalkCorrection(en_INDiE[1]);
            corrTimeDiff = corrTimeDiff - timeOffset;
            posSpectrumLin -> Fill(linPos->lin(corrTimeDiff, linPos->params.data()));

            posSpectrumPoly2 -> Fill(poly2Pos->getVelocity(corrTimeDiff)/2.0*corrTimeDiff);

            double poly3pos = poly2ThenFlatPos->poly3(corrTimeDiff, poly2ThenFlatPos->params.data());
            posSpectrumPoly2ThenFlat -> Fill(poly2ThenFlatPos->poly3(corrTimeDiff, poly2ThenFlatPos->params.data()));
            double corrTimeSum = t_INDiE[0] + wc->getWalkCorrection(en_INDiE[0]) - timeOffset + t_INDiE[1] + wc->getWalkCorrection(en_INDiE[1])-8000;
            correctedSumSpectrum -> Fill(corrTimeSum);
            if(poly3pos > position - 1 && poly3pos < position + 1){
                //correctedSumSpectrum -> Fill(corrTimeSum);
                /*cout << poly2ThenFlatPos->poly3(corrTimeDiff, poly2ThenFlatPos->params.data()) << endl;
                cout << "position: " << position << endl;
                cout << corrTimeDiff << endl;
                cout << t_INDiE[0] + wc->getWalkCorrection(en_INDiE[0]) << endl;
                cout << t_INDiE[1] + wc->getWalkCorrection(en_INDiE[1]) << endl;
                cout << corrTimeSum << endl;
                cout << endl;*/
            }
        }
    }
}

void VelocityMeasurement::fitHistograms() {
    correctedGaussFitter = new GaussFitter(correctedSpectrum);
    uncorrectedGaussFitter = new GaussFitter(uncorrectedSpectrum);
    vector<GaussFitter*> fitters = {correctedGaussFitter, uncorrectedGaussFitter};
    for(auto fitter : fitters){
        fitter->extractParameters();
        fitter->fitGaussianLin(fitter->maxPos - 1*fitter->FWHM, fitter->maxPos + 1*fitter->FWHM);
    }
}

void VelocityMeasurement::fitPositionHistograms() {
    posGaussFitterLin = new GaussFitter(posSpectrumLin);
    posGaussFitterPoly2 = new GaussFitter(posSpectrumPoly2);
    posGaussFitterPoly2ThenFlat = new GaussFitter(posSpectrumPoly2ThenFlat);
    vector<GaussFitter*> fitters = {posGaussFitterLin, posGaussFitterPoly2, posGaussFitterPoly2ThenFlat};
    for(auto fitter : fitters){
        fitter->extractParameters();
        fitter->fitGaussianLin(fitter->maxPos - 1*fitter->FWHM, fitter->maxPos + 1*fitter->FWHM);
    }
}
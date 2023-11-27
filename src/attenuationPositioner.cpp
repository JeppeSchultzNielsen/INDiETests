//
// Created by jeppe on 10/27/23.
//

#include "attenuationPositioner.h"

void AttenuationPositioner::setaParams(vector<double> aParams) {
    this->aParams = aParams;
}

void AttenuationPositioner::setbParams(vector<double> bParams) {
    this->bParams = bParams;
}

void AttenuationPositioner::saveParams(TString fileName) {
    ofstream outFile;
    outFile.open(fileName);
    for (int i = 0; i < aParams.size(); ++i) {
        cout << "Saving " << aParams[i] << " " << bParams[i] << endl;
        outFile << aParams[i] << " " << bParams[i] << endl;
    }
    outFile.close();
}

void AttenuationPositioner::loadParams(TString fileName) {
    ifstream inFile;
    inFile.open(fileName);
    double a, b;
    while (inFile >> a >> b){
        aParams.push_back(a);
        bParams.push_back(b);
    }
    inFile.close();
}

double AttenuationPositioner::getPosition(double qdc_r, double qdc_l) {
    //cout << "expected: " << (aParams[0] + aParams[1]*55 + aParams[2]*55*55)*qdc_l + (bParams[0] + bParams[1]*55 + bParams[2]*55*55)*qdc_l*qdc_l << endl;
    //cout << "measured: " << qdc_r << endl;
    // we have qdc_r = (aParams[0] + aParams[1]*x + aParams[2]*x*x)*qdc_l + (bParams[0] + bParams[1]*x + bParams[2]*x*x)*qdc_l*qdc_l and must solve for x
    double alpha = (aParams[0]*qdc_l + bParams[0]*qdc_l*qdc_l - qdc_r);
    double beta = (aParams[1]*qdc_l + bParams[1]*qdc_l*qdc_l);
    double gamma = (aParams[2]*qdc_l + bParams[2]*qdc_l*qdc_l);
    double delta = beta*beta - 4*alpha*gamma;
    if(delta < 0){
        cout << "Delta is negative, returning -1000" << endl;
        return -1000;
    }
    double x1 = (-beta + sqrt(delta))/(2*gamma);
    double x2 = (-beta - sqrt(delta))/(2*gamma);
    return x1;
}

void AttenuationPositioner::attenuationTest(TString fileName, TString saveTo) {
    TFile* file = TFile::Open(fileName,"READ");
    TTree* tree = static_cast<TTree*>(file->Get("ids"));

    double en_INDiE[2];
    double t_INDiE[2];

    tree->SetBranchAddress("TraceIntegral_INDiE", en_INDiE);
    tree->SetBranchAddress("Time_INDiE", t_INDiE);

    auto attenuationHist = new TH2D("Pos vs QDC", "Pos vs QDC", 100, -100, 100, 100, 0, 300e3);
    auto positionHist = new TH1D("Pos spectrum", "Pos spectrum",500, -100, 100);

    int nEntries = tree->GetEntries();
    for(int i = 0; i < nEntries; i++){
        tree->GetEntry(i);
        if(en_INDiE[0] > 0 && en_INDiE[1] > 0){
            //getPosition(142500,81600);
            double x = getPosition(en_INDiE[1], en_INDiE[0]);
            attenuationHist ->Fill(x, en_INDiE[1]);
            positionHist -> Fill(x);
        }
    }

    //save histogram
    TFile* outFile = TFile::Open(saveTo, "RECREATE");
    attenuationHist->Write();
    positionHist -> Write();
    outFile->Close();
}
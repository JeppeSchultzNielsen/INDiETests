//
// Created by jeppe on 10/27/23.
//

#ifndef INDIETESTS_ATTENUATIONPOSITIONER_H
#define INDIETESTS_ATTENUATIONPOSITIONER_H

#include "vector"
#include "TString.h"
#include "iostream"
#include "fstream"
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
using namespace std;

class AttenuationPositioner{
public:
    vector<double> aParams;
    vector<double> bParams;

    AttenuationPositioner(){}

    double getPosition(double e_r, double e_l);

    void loadParams(TString fileName);

    void saveParams(TString fileName);

    void setaParams(vector<double> aParams);

    void setbParams(vector<double> bParams);

    void attenuationTest(TString fileName, TString saveTo);

};

#endif //INDIETESTS_ATTENUATIONPOSITIONER_H

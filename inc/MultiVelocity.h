//
// Created by jeppe on 11/8/23.
//

#ifndef INDIETESTS_MULTIVELOCITY_H
#define INDIETESTS_MULTIVELOCITY_H

#include "VelocityMeasurement.h"
#include "Positioner.hh"
#include "TGraphAsymmErrors.h"

class MultiVelocity {
public:
    TString inputNameCycle;
    vector<int> runNumbers;
    vector<double> positions;
    vector<VelocityMeasurement*> velocityMeasurements;
    TGraph *correctedSigmas;
    TGraph *uncorrectedSigmas;
    TGraphAsymmErrors *speedOfLightGraph;
    TGraphErrors *timeGraph;
    TMultiGraph *residualGraph;
    TGraphErrors *residualsLin;
    TGraphErrors *residualsPoly2;
    TGraphErrors *residualsPoly2ThenFlat;
    TMultiGraph *allSigmas;
    TGraphErrors *edgeDiffGraph;
    TMultiGraph *speedOfLightCanvas;
    TMultiGraph *reconstructedPositionResiduals;
    TMultiGraph *reconstructedPositionSigmas;

    MultiVelocity(vector<int> runNumbers, vector<double> positions, TString inputNameCycle){
        this-> runNumbers = runNumbers;
        this-> positions = positions;
        this-> inputNameCycle = inputNameCycle;
        for (int i = 0; i < runNumbers.size(); ++i) {
            velocityMeasurements.push_back(new VelocityMeasurement(runNumbers[i], positions[i]));
        }
    }

    void doAnalyses();

    void saveHistograms(TString fileName);
};

#endif //INDIETESTS_MULTIVELOCITY_H

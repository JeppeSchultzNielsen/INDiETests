//
// Created by jeppe on 10/25/23.
//

#ifndef NEUTRONTOF_MULTIATTENUATION_H
#define NEUTRONTOF_MULTIATTENUATION_H

#include "attenuation.hh"
#include "TMultiGraph.h"
#include "attenuationPositioner.h"
using namespace std;

class MultiAttenuation{
public:
    vector<AttenuationAnalysis *> attenuationAnalyses;
    TString inputName;
    TString outputName;
    TMultiGraph *allGraph;
    vector<double> as;
    vector<double> bs;
    TGraph *aGraph;
    TGraph *bGraph;
    AttenuationPositioner *attenuationPositioner;

    MultiAttenuation(vector<int> runNums, vector<double> positions, TString inputName, TString outpuName){
        this -> inputName = inputName;
        this -> outputName = outpuName;
        for (int i = 0; i < runNums.size(); ++i) {
            attenuationAnalyses.push_back( new AttenuationAnalysis(runNums[i], positions[i]) );
        }
        attenuationPositioner = new AttenuationPositioner();
    }

    void createMultiGraph();

    void saveMultiGraph(TString fileName);

    static double abFun(double *x, double *par);

    void fitabFun(TGraph *g, TString type);

    void doAnalyses();
};

#endif //NEUTRONTOF_MULTIATTENUATION_H

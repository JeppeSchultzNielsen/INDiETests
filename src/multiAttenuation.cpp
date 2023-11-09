//
// Created by jeppe on 10/25/23.
//

#include "multiAttenuation.h"

void MultiAttenuation::doAnalyses(){
    for (int i = 0; i < attenuationAnalyses.size(); ++i) {
        attenuationAnalyses[i] -> loopOverFile(Form(inputName, attenuationAnalyses[i] -> runNum));
        attenuationAnalyses[i] -> createProjections();
        attenuationAnalyses[i] -> fitProjections();
        auto ab = attenuationAnalyses[i] -> fitScale();
        as.push_back(ab.first);
        bs.push_back(ab.second);
        attenuationAnalyses[i] -> saveHistograms(Form(outputName, attenuationAnalyses[i] -> runNum));
    }
}

void MultiAttenuation::createMultiGraph(){
    allGraph = new TMultiGraph();
    aGraph = new TGraph();
    bGraph = new TGraph();
    for (int i = 0; i < attenuationAnalyses.size(); ++i) {
        aGraph -> SetPoint(i, attenuationAnalyses[i]->position, as[i]);
        bGraph -> SetPoint(i, attenuationAnalyses[i]->position, bs[i]);
        attenuationAnalyses[i]->attenuationGraph -> SetMarkerStyle(i+20);
        attenuationAnalyses[i]->attenuationGraph -> SetMarkerColor(i+1);
        allGraph -> Add(attenuationAnalyses[i]->attenuationGraph);
    }
    fitabFun(aGraph,"a");
    aGraph ->SetMarkerStyle(20);
    bGraph ->SetMarkerStyle(20);
    fitabFun(bGraph,"b");
    attenuationPositioner -> saveParams("output/attenuationParams.txt");
}

void MultiAttenuation::saveMultiGraph(TString fileName){
    //create new rootfile and save the graphs
    TFile *f = new TFile(fileName, "RECREATE");
    allGraph -> Write("allGraph");
    aGraph -> Write("aGraph");
    bGraph -> Write("bGraph");
    f -> Close();
}

double MultiAttenuation::abFun(double *x, double *par) {
    return par[0] + par[1]*x[0] + par[2]*x[0]*x[0];
}

void MultiAttenuation::fitabFun(TGraph *g, TString type) {
    TF1 *f = new TF1("f", abFun, 0, 100, 3);
    f -> SetParameters(1,1,1);
    g -> Fit(f);

    vector<double> params = {f->GetParameter(0), f->GetParameter(1), f->GetParameter(2)};
    if(type == "a"){
        attenuationPositioner -> setaParams(params);
    }
    if(type == "b"){
        attenuationPositioner -> setbParams(params);
    }
}
//
// Created by jeppe on 11/12/23.
//

#include "TracePlotter.hh"

void TracePlotter::loopOverFile(TString fileName) {
    file = TFile::Open(fileName);

    TTree* tree = static_cast<TTree*>(file->Get("ids"));

    int traceLen = 130;

    unsigned int traces_INDiE[2][traceLen];
    double En_INDiE[2];
    double T_INDiE[2];

    traceLen = 120;

    int bins = 500;
    double lowEdge = -5;
    double highEdge = 20;
    avgTraceL = new TH1D("avgTraceL","avgTraceL",bins,lowEdge,highEdge);
    avgTraceR = new TH1D("avgTraceR","avgTraceR",bins,lowEdge,highEdge);
    stdTraceL = new TH1D("stdTraceL","stdTraceL",bins,lowEdge,highEdge);
    stdTraceR = new TH1D("stdTraceR","stdTraceR",bins,lowEdge,highEdge);
    avgTraceL = new TH1D("avgTraceLFree","avgTraceLFree",bins,lowEdge,highEdge);
    avgTraceR = new TH1D("avgTraceRFree","avgTraceRFree",bins,lowEdge,highEdge);
    stdTraceL = new TH1D("stdTraceLFree","stdTraceLFree",bins,lowEdge,highEdge);
    stdTraceR = new TH1D("stdTraceRFree","stdTraceRFree",bins,lowEdge,highEdge);
    frontBackRatiosL = new TH1D(Form("frontBackRatiosL %f",position),Form("frontBackRatiosL %f",position),100,-2,5);
    frontBackRatiosR = new TH1D(Form("frontBackRatiosR %f",position),Form("frontBackRatiosR %f",position),100,-2,5);



    tree->SetBranchAddress("Trace_INDiE", &traces_INDiE);
    tree->SetBranchAddress("Time_INDiE", &T_INDiE);
    tree->SetBranchAddress("TraceIntegral_INDiE", &En_INDiE);

    Long64_t numEntries = tree->GetEntries();
    numEntries = 100;
    vector<int> countL = {};
    vector<int> countR = {}; // the amount of times each bin has been incremented
    for(int i = 0; i < avgTraceL -> GetNbinsX(); i++){
        countL.push_back(0);
        countR.push_back(0);
    }

    vector<unsigned int> traceL = {};
    vector<unsigned int> traceR = {};
    vector<double> betaParams = {3.83585e-01};
    vector<double> gammaParams = {2.82287e-01};

    auto dig_daq_param_left = new DigDaqParamINDiE(0, 0, "INDiE", betaParams, gammaParams, false);
    auto dig_daq_param_right = new DigDaqParamINDiE(0, 0, "INDiE", betaParams, gammaParams, false);

    auto tw = new TimeWriter(10000);
    tw->start();
    vector<double> phasesL = {};
    vector<double> phasesR = {};
    vector<Long64_t> entries = {};
    vector<TGraph*> traceGraphsL = {};
    vector<TGraph*> traceGraphsR = {};
    for (Long64_t entry = 0; entry < numEntries; ++entry) {
        tw->writeTime(entry,numEntries);
        tree->GetEntry(entry);
        if(En_INDiE[0] < 1 or En_INDiE[1] < 1){
            continue;
        }

        traceL = {};
        traceR = {};

        for(int i = 0; i < traceLen; i++){
            traceL.push_back(traces_INDiE[0][i]);
            traceR.push_back(traces_INDiE[1][i]);
        }

        Trace *traceObjL = new Trace(traceL);
        Trace *traceObjR = new Trace(traceR);
        traceObjL -> findTraceParams();
        traceObjL -> subtractBaseline();
        traceObjR -> findTraceParams();
        traceObjR -> subtractBaseline();
        if(traceObjR -> baseline < 6600 or traceObjR -> baseline > 6800 or traceObjL -> baseline < 7000 or traceObjL -> baseline > 7100){
            continue;
        }

        double frontL = 0;
        double frontR = 0;
        double backL = 0;
        double backR = 0;


        auto phaseAlphaL = dig_daq_param_left ->calculatePhase(traceObjL);
        auto phaseAlphaR = dig_daq_param_right ->calculatePhase(traceObjR);
        auto phaseL = phaseAlphaL.first;
        auto phaseR = phaseAlphaR.first;
        phasesL.push_back(phaseL);
        phasesR.push_back(phaseR);
        entries.push_back(entry);

        for(int k = 0; k < traceObjR -> data.size(); k++) {
            int bin = avgTraceR -> FindBin(k - phaseR);
            if(bin == 0) continue;
            avgTraceR -> AddBinContent(bin, (traceObjR -> data[k])/(traceObjR -> max));
            countR[bin]++;
        }

        for(int k = 0; k < traceObjL -> data.size(); k++) {
            int bin = avgTraceL -> FindBin(k - phaseL);
            if(bin == 0) continue;
            avgTraceL -> AddBinContent(bin, (traceObjL -> data[k])/(traceObjL -> max));
            countL[bin]++;
        }

        if(entry < 100){
            TGraph* newGraphL = new TGraph();
            newGraphL -> SetName(Form("L entry %i",entry));
            TGraph* newGraphR = new TGraph();
            newGraphR -> SetName(Form("R entry %i",entry));
            traceGraphsL.push_back(newGraphL);
            traceGraphsR.push_back(newGraphR);
            for(int k = 0; k < traceObjL -> data.size(); k++) {
                traceGraphsL.back()->AddPoint(k - phaseL, traceObjL -> data[k] / traceObjL->max);
            }
            for(int k = 0; k < traceObjR -> data.size(); k++) {
                traceGraphsR.back()->AddPoint(k - phaseR, traceObjR -> data[k]/ traceObjR->max);
            }
        }

        delete traceObjL;
        delete traceObjR;
    }

    for(int i = 0; i < avgTraceL->GetNbinsX(); i++){
        avgTraceL -> SetBinContent(i, avgTraceL->GetBinContent(i)/max(countL[i],1));
        avgTraceR -> SetBinContent(i, avgTraceR->GetBinContent(i)/max(countR[i],1));
    }

    //do the same, this time calculating std
    for(int i = 0; i < avgTraceL -> GetNbinsX(); i++){
        countL.push_back(0);
        countR.push_back(0);
    }
    tw->start();

    for (auto entry : entries) {
        tw->writeTime(entry,entries.size());
        tree->GetEntry(entry);
        if(En_INDiE[0] < 1 or En_INDiE[1] < 1){
            continue;
        }

        traceL = {};
        traceR = {};

        for(int i = 0; i < traceLen; i++){
            traceL.push_back(traces_INDiE[0][i]);
            traceR.push_back(traces_INDiE[1][i]);
        }

        Trace *traceObjL = new Trace(traceL);
        Trace *traceObjR = new Trace(traceR);
        traceObjL -> findTraceParams();
        traceObjL -> subtractBaseline();
        traceObjR -> findTraceParams();
        traceObjR -> subtractBaseline();

        for(int k = 0; k < traceObjR -> data.size(); k++) {
            int bin = avgTraceR -> FindBin(k - phasesR[entry]);
            if(bin == 0) continue;
            stdTraceR -> AddBinContent(bin, pow( (traceObjR -> data[k])/(traceObjR -> max) - avgTraceR->GetBinContent(bin),2));
            countR[bin]++;
        }

        for(int k = 0; k < traceObjL -> data.size(); k++) {
            int bin = avgTraceL -> FindBin(k - phasesL[entry]);
            if(bin == 0) continue;
            stdTraceL -> AddBinContent(bin, pow( (traceObjL -> data[k])/(traceObjL -> max) - avgTraceL->GetBinContent(bin),2));
            countL[bin]++;
        }

    }

    for(int i = 0; i < avgTraceL -> GetNbinsX(); i++){
        stdTraceL -> SetBinContent(i, sqrt(stdTraceL->GetBinContent(i)/ max(countL[i],1)));
        stdTraceR -> SetBinContent(i, sqrt(stdTraceR->GetBinContent(i)/ max(countR[i],1)));
    }

    tracesL = new TMultiGraph();
    tracesR = new TMultiGraph();
    for(int i = 0; i < traceGraphsL.size(); i++){
        tracesL -> Add(traceGraphsL[i]);
        tracesR -> Add(traceGraphsR[i]);
    }
    tracesL->GetXaxis()->SetRangeUser(0,15);
    tracesR->GetXaxis()->SetRangeUser(0,15);
    tracesR->GetYaxis()->SetRangeUser(-0.1,1.1);
    tracesL->GetYaxis()->SetRangeUser(-0.1,1.1);
}

void TracePlotter::createHistograms() {
    traceLGraph = new TGraphErrors();
    traceRGraph = new TGraphErrors();

    for(int i = 0; i < stdTraceL -> GetNbinsX(); i++){
        traceLGraph->SetPoint(i,avgTraceL ->GetBinCenter(i),avgTraceL ->GetBinContent(i));
        traceLGraph->SetPointError(i,0,stdTraceL ->GetBinContent(i));
    }
    for(int i = 0; i < stdTraceR -> GetNbinsX(); i++){
        traceRGraph->SetPoint(i,avgTraceR ->GetBinCenter(i),avgTraceR ->GetBinContent(i));
        traceRGraph->SetPointError(i,0,stdTraceR ->GetBinContent(i));
    }

    //set limits on the graphs
    traceLGraph->GetXaxis()->SetRangeUser(0,10);
    traceRGraph->GetXaxis()->SetRangeUser(0,10);
    traceLGraph->GetYaxis()->SetRangeUser(-0.3,1.3);
    traceRGraph->GetYaxis()->SetRangeUser(-0.3,1.3);
}
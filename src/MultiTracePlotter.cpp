//
// Created by jeppe on 11/12/23.
//

#include "MultiTracePlotter.hh"

void MultiTracePlotter::doAnalyses() {
    for (int i = 0; i < runNumbers.size(); ++i) {
        cout << "Analyzing run " << runNumbers[i] << " at position " << positions[i] << endl;
        auto tp = new TracePlotter(positions[i]);
        tracePlotters.push_back(tp);
        tp->loopOverFile(Form(inputNameCycle.Data(), runNumbers[i]));
        tp -> createHistograms();
    }
}

void MultiTracePlotter::doAnalysesSingle() {
    for (int i = 0; i < runNumbers.size(); ++i) {
        cout << "Analyzing run " << runNumbers[i] << " at position " << positions[i] << endl;
        auto tp = new TracePlotter(positions[i]);
        tracePlotters.push_back(tp);
        tp->loopOverFileSingle(Form(inputNameCycle.Data(), runNumbers[i]));
        tp -> createHistogramSingle();
    }
}

void MultiTracePlotter::saveHistograms(TString fileName) {
    TFile *file = new TFile(fileName, "RECREATE");
    file->cd();
    for (int i = 0; i < runNumbers.size(); ++i) {
        TDirectory *dir = file->mkdir(Form("Traces position %f", positions[i]));
        dir->cd();
        tracePlotters[i]->traceLGraph->Write("traceLGraph");
        tracePlotters[i]->traceRGraph->Write("traceRGraph");
        tracePlotters[i]->frontBackRatiosL->Write();
        tracePlotters[i]->frontBackRatiosR->Write();
        tracePlotters[i]->tracesL->Write();
        tracePlotters[i]->tracesR->Write();
        tracePlotters[i]->file->Close();
    }

    file->Close();
}
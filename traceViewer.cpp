#include "TFile.h"
#include "TTree.h"
#include "vector"
#include "TGraph.h"
#include "TH1D.h"

#include "iostream"

using namespace std;

void writeTracesToFile(TString fileName, double leftFrom, double leftTo, double rightFrom, double rightTo, TString saveTo){
    TFile* file = TFile::Open(fileName,"READ");
    TTree* tree = static_cast<TTree*>(file->Get("ids"));

    double en_INDiE[2];
    double t_INDiE[2];
    unsigned int trace_INDiE[2][130];
    double baseline[2];

    tree->SetBranchAddress("TraceIntegral_INDiE", en_INDiE);
    tree->SetBranchAddress("Energy_INDiE", baseline);
    tree->SetBranchAddress("Time_INDiE", t_INDiE);
    tree->SetBranchAddress("Trace_INDiE", trace_INDiE);

    vector<TH1D *> leftTraces;
    vector<TH1D *> rightTraces;
    vector<double> leftEnergies;
    vector<double> rightEnergies;
    vector<double> leftBaseline;
    vector<double> rightBaseline;

    int nEntries = tree->GetEntries();
    for(int i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        if (en_INDiE[0] > rightFrom && en_INDiE[0] < rightTo && en_INDiE[1] > leftFrom && en_INDiE[1] < leftTo) {
            leftEnergies.push_back(en_INDiE[1]);
            rightEnergies.push_back(en_INDiE[0]);
            leftTraces.push_back(new TH1D(Form("Left: %f", en_INDiE[1]), Form("Left: %f", en_INDiE[1]), 130, 0, 130));
            rightTraces.push_back(new TH1D(Form("Right: %f", en_INDiE[0]), Form("Right: %f",en_INDiE[0]), 130, 0, 130));
            leftBaseline.push_back(baseline[1]);
            rightBaseline.push_back(baseline[0]);
            for(int j = 0; j < 130; j++){
                leftTraces[leftTraces.size() - 1]->SetBinContent(j,trace_INDiE[1][j]);
                rightTraces[rightTraces.size() - 1]->SetBinContent(j,trace_INDiE[0][j]);
            }
        }
    }

    TFile* saveFile = TFile::Open(saveTo, "RECREATE");
    cout << leftEnergies.size() << endl;
    for(int i = 0; i < leftEnergies.size(); i++){
        TDirectory *dir = saveFile -> mkdir(Form("Left: %f, Right: %f", leftEnergies[i], rightEnergies[i]));
        dir -> cd();
        leftTraces[i] -> Write(Form("Left, baseline %f", leftBaseline[i]));
        rightTraces[i] -> Write(Form("Right, baseline %f", rightBaseline[i]));
    }
    saveFile -> Close();
}

int main(){
    writeTracesToFile("UnpackedINDiETests/Run014Traces.root",0,10e3,50e3,150e3, "output/14traces.root");
}
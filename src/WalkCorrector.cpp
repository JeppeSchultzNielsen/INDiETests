//
// Created by jeppe on 10/27/23.
//

#include "WalkCorrector.h"

void WalkCorrector::loopOverFile(TString fileName) {
    TFile* file = TFile::Open(fileName,"READ");
    TTree* tree = static_cast<TTree*>(file->Get("ids"));

    double en_INDiE[2];
    double t_INDiE[2];

    tree->SetBranchAddress("TraceIntegral_INDiE", en_INDiE);
    tree->SetBranchAddress("Time_INDiE", t_INDiE);

    QDCvsQDC = new TH2D("Attenuation Hist", "Attenuation Hist", 300, 0, 300e3, 300, 0, 300e3);

    int nEntries = tree->GetEntries();
    for(int i = 0; i < nEntries; i++){
        tree->GetEntry(i);
        if(en_INDiE[0] > 0 && en_INDiE[1] > 0){
            if(t_INDiE[0] - t_INDiE[1] > 1 && t_INDiE[0] - t_INDiE[1] < 5){
                QDCvsQDC->Fill(en_INDiE[0], en_INDiE[1]);
            }
        }
    }

    int minNum = 100;
    //create a histogram for each bin in the QDCvsQDC histogram that has more than 100 entries
    for(int i = 0; i < QDCvsQDC->GetNbinsX(); i++){
        if(i == 0){
            dL = QDCvsQDC->GetXaxis()->GetBinWidth(i);
        }
        for(int j = 0; j < QDCvsQDC->GetNbinsY(); j++){
            if(j == 0){
                dR = QDCvsQDC->GetYaxis()->GetBinWidth(j);
            }
            if(QDCvsQDC->GetBinContent(i, j) > minNum){
                walkTH1s.push_back(new TH1D(Form("L %f R %f", QDCvsQDC->GetXaxis()->GetBinLowEdge(i), QDCvsQDC->GetYaxis()->GetBinLowEdge(j)), Form("L %f R %f", QDCvsQDC->GetXaxis()->GetBinLowEdge(i), QDCvsQDC->GetYaxis()->GetBinLowEdge(j)), 40, 1, 5));
                lowerEdges.push_back(make_pair(QDCvsQDC->GetXaxis()->GetBinLowEdge(i), QDCvsQDC->GetYaxis()->GetBinLowEdge(j)));
            }
        }
    }

    //fill the histograms
    TimeWriter *tw = new TimeWriter(1e4);
    tw -> start();
    for(int i = 0; i < nEntries; i++){
        tree->GetEntry(i);
        tw ->writeTime(i,nEntries);
        if(en_INDiE[0] > 0 && en_INDiE[1] > 0){
            if(t_INDiE[0] - t_INDiE[1] > 1 && t_INDiE[0] - t_INDiE[1] < 5){
                for(int j = 0; j < walkTH1s.size(); j++){
                    if(en_INDiE[0] > lowerEdges[j].first && en_INDiE[0] < lowerEdges[j].first + dL && en_INDiE[1] > lowerEdges[j].second && en_INDiE[1] < lowerEdges[j].second + dR){
                        walkTH1s[j]->Fill(t_INDiE[0] - t_INDiE[1]);
                        continue;
                    }
                }
            }
        }
    }

    //remove the histograms which have shoulders on the left
    for(int i = 0; i < walkTH1s.size(); i++){
        double avgFirst10 = 0;
        double maxAfterFirst10 = 0;
        for(int j = 0; j < walkTH1s[i]->GetNbinsX(); j++) {
            if (j < 10) {
                avgFirst10 += walkTH1s[i]->GetBinContent(j);
            }
            else{
                if(walkTH1s[i]->GetBinContent(j) > maxAfterFirst10){
                    maxAfterFirst10 = walkTH1s[i]->GetBinContent(j);
                }
            }
        }
        avgFirst10 /= 10;
        if(0.2 * maxAfterFirst10 < avgFirst10){
            walkTH1s.erase(walkTH1s.begin() + i);
            lowerEdges.erase(lowerEdges.begin() + i);
            i--;
        }
    }
}

void WalkCorrector::fitWalkTH1s(){
    for(int i = 0; i < walkTH1s.size(); i++){
        auto gf = new GaussFitter(walkTH1s[i]);
        gf -> extractParameters();
        gf -> fitGaussian(2,5);
        walkMeans.push_back(gf->fitMean/4);
        walkSigmas.push_back(gf->fitMeanErr/4);
    }

    //fill the walkHist
    walkHist = new TH2D("Walk plot", "Walk plot", 300, 0, 300e3, 300, 0, 300e3);
    for(int i = 0; i < walkTH1s.size(); i++){
        walkHist->Fill(lowerEdges[i].first + dL/2, lowerEdges[i].second + dR/2, walkMeans[i]-3.0/4.0);
    }
}

void WalkCorrector::saveHistograms(TString saveTo) {
    TFile* file = TFile::Open(saveTo, "RECREATE");
    QDCvsQDC->Write();
    TDirectory *dir = file -> mkdir("Walks");
    dir -> cd();
    for(int i = 0; i < walkTH1s.size(); i++){
        walkTH1s[i]->Write();
    }
    file -> cd();
    walkHist -> Write();
    file->Close();
}
//
// Created by jeppe on 10/23/23.
//

#include "attenuation.hh"

void AttenuationAnalysis::loopOverFile(TString fileName) {
    TFile* file = TFile::Open(fileName,"READ");
    TTree* tree = static_cast<TTree*>(file->Get("ids"));

    double en_INDiE[2];
    double t_INDiE[2];

    tree->SetBranchAddress("TraceIntegral_INDiE", en_INDiE);
    tree->SetBranchAddress("Time_INDiE", t_INDiE);

    attenuationHist = new TH2D("Attenuation Hist", "Attenuation Hist", 100, 0, 300e3, 100, 0, 300e3);

    int nEntries = tree->GetEntries();
    for(int i = 0; i < nEntries; i++){
        tree->GetEntry(i);
        if(en_INDiE[0] > 0 && en_INDiE[1] > 0){
            attenuationHist->Fill(en_INDiE[0], en_INDiE[1]);
        }
    }
}

void AttenuationAnalysis::createProjections() {
    for(int i = 0; i < 1000; i++){
        attenuationProjections.push_back(attenuationHist->ProjectionX(Form("Projection %d", i), i, i));
    }
}

double AttenuationAnalysis::findComptonPeak(){
    //create histogram that is projection of all bins
    auto proj = attenuationHist ->ProjectionY();
    //find the maximum bin a minimum energy of 50e3
    int maxVal = 0;
    double maxE = 0;
    for(int i = 0; i < proj->GetNbinsX(); i++){
        if(proj ->GetBinCenter(i) < 50e3) continue;
        if(proj ->GetBinContent(i) > maxVal){
            maxVal = proj ->GetBinContent(i);
            maxE = proj ->GetBinCenter(i);
        }
    }
    return maxE;
}

void AttenuationAnalysis::fitProjections() {

    for(int i = 0; i < 1000; i++){
        double er = attenuationHist->GetYaxis()->GetBinCenter(i);
        double ce = findComptonPeak();
        if(er > ce or attenuationProjections[i] -> GetEntries() < 1000) continue;
        e_r.push_back(er);
        auto gf = new GaussFitter(attenuationProjections[i]);
        gf->fitGaussian(gf->maxPos - gf -> FWHM, gf->maxPos + gf -> FWHM);
        e_l.push_back(gf->fitMean);
    }
    attenuationGraph = new TGraph();
    attenuationGraph -> SetNameTitle("Attenuation Graph", "Attenuation Graph");
    for(int i = 0; i < e_r.size(); i++){
        attenuationGraph -> AddPoint( e_l[i],e_r[i]);
    }
    projectionsFitted = true;
}

pair<double,double> AttenuationAnalysis::fitScale(){
    //fit y=a*x to the attenuation graph
    auto lin = new TF1("lin","[0]*x + [1]*x*x",0,300e3);
    attenuationGraph -> Fit(lin,"QR","",0,300e3);
    return make_pair(lin ->GetParameter(0),lin ->GetParameter(1)) ;
}

void AttenuationAnalysis::saveHistograms(TString saveTo) {
    TFile* file = TFile::Open(saveTo, "RECREATE");
    attenuationHist->Write();
    if(projectionsFitted){
        attenuationGraph->Write();
    }
    file -> mkdir("Projections");
    file -> cd("Projections");
    for(auto &attenuationProjection : attenuationProjections){
        attenuationProjection->Write();
    }
    file->Close();
}
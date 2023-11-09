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

    /*int minNum = 100;
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
    }*/
    //create a histogram the maximum bin for each QDC_R value
    for(int i = 0; i < QDCvsQDC->GetNbinsY(); i++){
        if(i == 0){
            dR = QDCvsQDC->GetYaxis()->GetBinWidth(i);
            dL = QDCvsQDC->GetXaxis()->GetBinWidth(i);
        }
        double maxBin = 0;
        int maxBinIndex = 0;
        for(int j = 0; j < QDCvsQDC->GetNbinsX(); j++){
            if(QDCvsQDC->GetBinContent(j, i) > maxBin){
                maxBin = QDCvsQDC->GetBinContent(j, i);
                maxBinIndex = j;
            }
        }
        if(QDCvsQDC -> GetYaxis()->GetBinLowEdge(i) > 150e3 or QDCvsQDC -> GetYaxis()->GetBinLowEdge(i) < 3e3){
            continue;
        }
        walkTH1s.push_back(new TH1D(Form("R %f", QDCvsQDC->GetYaxis()->GetBinLowEdge(i)), Form("R %f", QDCvsQDC->GetYaxis()->GetBinLowEdge(i)), 40, 4, 20));
        lowerEdges.push_back(make_pair(QDCvsQDC->GetXaxis()->GetBinLowEdge(maxBinIndex), QDCvsQDC->GetYaxis()->GetBinLowEdge(i)));
    }

    //fill the histograms
    TimeWriter *tw = new TimeWriter(1e4);
    tw -> start();
    for(int i = 0; i < nEntries; i++){
        tree->GetEntry(i);
        tw ->writeTime(i,nEntries);
        if(en_INDiE[0] > 0 && en_INDiE[1] > 0){
            t_INDiE[0] = 4.0*t_INDiE[0];
            t_INDiE[1] = 4.0*t_INDiE[1];
            if(t_INDiE[0] - t_INDiE[1] > 4 && t_INDiE[0] - t_INDiE[1] < 20){
                for(int j = 0; j < walkTH1s.size(); j++){
                    if(en_INDiE[0] > lowerEdges[j].first && en_INDiE[0] < lowerEdges[j].first + dL && en_INDiE[1] > lowerEdges[j].second && en_INDiE[1] < lowerEdges[j].second + dR){
                        if(loadedWalkPars){
                            walkTH1s[j]->Fill(t_INDiE[0] - t_INDiE[1] + getWalkCorrection(en_INDiE[0]) - getWalkCorrection(en_INDiE[1]));
                        }
                        else{
                            walkTH1s[j]->Fill(t_INDiE[0] - t_INDiE[1]);
                        }
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
        if(0.05 * maxAfterFirst10 < avgFirst10){
            walkTH1s.erase(walkTH1s.begin() + i);
            lowerEdges.erase(lowerEdges.begin() + i);
            i--;
        }
    }
}

void WalkCorrector::fitWalkTH1s(){
    double zeroPoint = 0;
    for(int i = 0; i < walkTH1s.size(); i++){
        auto gf = new GaussFitter(walkTH1s[i]);
        gf -> extractParameters();
        gf -> fitGaussian(10,16);
        if(isnan(gf -> fitMeanErr)) continue;
        if(walkMeans.size() == 0){
            zeroPoint = gf->fitMean;
        }
        walkMeans.push_back(gf->fitMean - zeroPoint);
        walkSigmas.push_back(gf->fitMeanErr);
    }

    //fill the walkHist
    walkHist = new TH2D("Walk plot", "Walk plot", 300, 0, 300e3, 300, 0, 300e3);
    for(int i = 0; i < walkTH1s.size(); i++){
        walkHist->Fill(lowerEdges[i].first + dL/2.0, lowerEdges[i].second + dR/2.0, walkMeans[i]);
    }

    for(int i = 0; i < lowerEdges.size(); i++){
        centersL.push_back(lowerEdges[i].first + dL/2.0);
        centersR.push_back(lowerEdges[i].second + dR/2.0);
    }

}

void WalkCorrector::saveVectors(TString saveTo) {
    //save the contents of the vectors to a txt file
    ofstream file;
    file.open(saveTo);
    for(int i = 0; i < walkMeans.size(); i++){
        file << centersL[i] << " " << centersR[i] << " " << walkMeans[i] << " " << walkSigmas[i] << endl;
    }
}

void WalkCorrector::loadVectors(TString fileName) {
    //load the contents of the txt file into the vectors
    ifstream file;
    file.open(fileName);
    centersL.clear();
    centersR.clear();
    walkMeans.clear();
    walkSigmas.clear();
    double l, r, m, s;
    while(file >> l >> r >> m >> s){
        centersL.push_back(l);
        centersR.push_back(r);
        walkMeans.push_back(m);
        walkSigmas.push_back(s);
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

double WalkCorrector::getWalkCorrection(double QDC, double *par) {
    double a = par[0];
    double b = par[1];
    double c = par[2];
    double d = par[3];
    return a*(1-exp(-QDC*b)) + c*QDC + d*QDC*QDC;
}

double WalkCorrector::getWalkCorrection(double QDC) {
    return walkPar[0]*(1-exp(-QDC*walkPar[1])) + walkPar[2]*QDC + walkPar[3]*QDC*QDC;
}

double WalkCorrector::toMinimize(const gsl_vector *v, void *params) {
    double wcPar[6] = {gsl_vector_get(v, 0), gsl_vector_get(v, 1), gsl_vector_get(v, 2), gsl_vector_get(v, 3)};
    //params is an a array of vectors of doubles
    vector<double> *paramsVec = (vector<double> *) params;
    vector<double> centersL = paramsVec[0];
    vector<double> centersR = paramsVec[1];
    vector<double> walkMeans = paramsVec[2];
    vector<double> walkSigmas = paramsVec[3];

    double sum = 0;
    for(int i = 0; i < centersL.size(); i++){
        sum += pow(walkMeans[i] - (getWalkCorrection(centersR[i], wcPar) - getWalkCorrection(centersL[i], wcPar)), 2);
    }
    return sum;
}

void WalkCorrector::doMinimization() {
    const gsl_multimin_fminimizer_type *T =
            gsl_multimin_fminimizer_nmsimplex2;
    gsl_multimin_fminimizer *s = NULL;
    gsl_vector *ss, *x;
    gsl_multimin_function minex_func;

    int nPar = 4;

    size_t iter = 0;
    int status;
    double size;

    /* Starting point */
    x = gsl_vector_alloc (nPar);

    gsl_vector_set (x, 0, 1.45829295e-01);
    gsl_vector_set (x, 1, 7.78386807e-05);
    gsl_vector_set (x, 2, 1.99495618e-06);
    gsl_vector_set(x,3,-6.78175224e-12);

    /* Set initial step sizes to 1 */
    ss = gsl_vector_alloc (nPar);
    gsl_vector_set (ss,0, 1.45829295e-02);
    gsl_vector_set (ss,1, 7.78386807e-06);
    gsl_vector_set (ss,2, 1.99495618e-07);
    gsl_vector_set (ss,3, -6.78175224e-13);

    vector<double> paramsVec[4] = {centersL, centersR, walkMeans, walkSigmas};
    /* Initialize method and iterate */
    minex_func.n = nPar;
    minex_func.f = toMinimize;
    minex_func.params = &paramsVec;

    s = gsl_multimin_fminimizer_alloc (T, nPar);
    gsl_multimin_fminimizer_set (s, &minex_func, x, ss);

    int maxiter = 10000;

    do
    {
        iter++;
        status = gsl_multimin_fminimizer_iterate(s);

        if (status)
            break;

        size = gsl_multimin_fminimizer_size (s);
        status = gsl_multimin_test_size (size, 1e-10);

        if (status == GSL_SUCCESS)
        {
            printf ("converged to minimum at\n");
        }

        printf ("%5d %10.3e %10.3e %10.3e %10.3e f() = %7.3e size = %.3e\n",
                iter,
                gsl_vector_get (s->x, 0),
                gsl_vector_get (s->x, 1),
                gsl_vector_get (s->x, 2),
                gsl_vector_get (s->x, 3),
                s->fval, size);
    }
    while (status == GSL_CONTINUE && iter < maxiter);
    cout << "finished loop" << endl;

    walkPar = {gsl_vector_get(s->x, 0), gsl_vector_get(s->x, 1), gsl_vector_get(s->x, 2), gsl_vector_get(s->x, 3)};
    cout << "wrote par" << endl;

    gsl_vector_free(x);
    gsl_vector_free(ss);
    gsl_multimin_fminimizer_free (s);
    cout << "finished minimization" << endl;
}

void WalkCorrector::writeWalkPars(TString saveTo) {
    ofstream file;
    file.open(saveTo);
    file << walkPar[0] << " " << walkPar[1] << " " << walkPar[2] << " " << walkPar[3] << endl;
    for(int i = 0; i < walkPar.size(); i++){
        cout <<  walkPar[i] << endl;
    }

}

void WalkCorrector::readWalkPars(TString fileName) {
    walkPar = {};
    ifstream file;
    file.open(fileName);
    double a,b,c,d;
    file >> a >> b >> c >> d;
    walkPar = {a,b,c,d};
    loadedWalkPars = true;
}
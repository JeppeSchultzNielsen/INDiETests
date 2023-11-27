//
// Created by jeppe on 11/8/23.
//

#include "MultiVelocity.h"

double static linear(double* x, double* par){
    return par[0]*x[0];
}

double static linearDerivative(double* x, double* par){
    return 2*par[0];
}

double static poly2(double* x, double* par){
    return par[0]*x[0]  + par[1]*x[0]*x[0];
}

double static poly2wBase(double* x, double* par){
    return par[0]  + par[1]*x[0] + par[2]*x[0]*x[0];
}

double static poly2Derivative(double* x, double* par){
    return 2*(par[0] + 2*par[1]*x[0]);
}

double static poly3(double* x, double* par){
    return par[0]*x[0]  + par[1]*x[0]*x[0] + par[2]*x[0]*x[0]*x[0];
}

double static poly3Derivative(double* x, double* par){
    return 2*(par[0]  + 2*par[1]*x[0] + 3*par[2]*x[0]*x[0]);
}

double static shoulderFun(double* x, double* par){
    double A = par[0];
    double mu = par[1];
    double sigma = par[2];
    double baseLow = par[3];

    double y = 0;
    if(x[0] > mu){
        y = A + baseLow;
    }
    else{
        y = A * exp(-0.5 * pow((x[0] - mu) / sigma, 2)) + baseLow;
    }

    return y;
}

void MultiVelocity::doAnalyses() {
    correctedSigmas = new TGraph();
    uncorrectedSigmas = new TGraph();
    allSigmas = new TMultiGraph();
    speedOfLightGraph = new TGraphAsymmErrors();
    timeGraph = new TGraphErrors();

    vector<double> times;
    vector<double> timeErrs;
    vector<double> edgeDiffs;
    vector<double> edgeDiffErrs;

    TGraph *positionSigmasLin = new TGraph();
    TGraph *positionSigmasPoly2 = new TGraph();
    TGraph *positionSigmasPoly3 = new TGraph();

    TGraph *positionResidualsLin = new TGraph();
    TGraph *positionResidualsPoly2 = new TGraph();
    TGraph *positionResidualsPoly3 = new TGraph();

    for (int i = 0; i < velocityMeasurements.size(); ++i) {

        velocityMeasurements[i]->createHistograms(Form(inputNameCycle.Data(), runNumbers[i]));
        velocityMeasurements[i]->fitHistograms();
        velocityMeasurements[i]->createPositionHistograms(Form(inputNameCycle.Data(), runNumbers[i]));
        velocityMeasurements[i]->fitPositionHistograms();
        correctedSigmas->SetPoint(i, velocityMeasurements[i]->position, velocityMeasurements[i]->correctedGaussFitter->fitSigma);
        uncorrectedSigmas->SetPoint(i, velocityMeasurements[i]->position, velocityMeasurements[i]->uncorrectedGaussFitter->fitSigma);
        positionSigmasLin -> SetPoint(positionSigmasLin->GetN(), velocityMeasurements[i]->position, velocityMeasurements[i]->posGaussFitterLin->fitSigma);
        positionSigmasPoly2 -> SetPoint(positionSigmasPoly2->GetN(), velocityMeasurements[i]->position, velocityMeasurements[i]->posGaussFitterPoly2->fitSigma);
        positionSigmasPoly3 -> SetPoint(positionSigmasPoly3->GetN(), velocityMeasurements[i]->position, velocityMeasurements[i]->posGaussFitterPoly2ThenFlat->fitSigma);
        positionResidualsLin -> SetPoint(positionResidualsLin->GetN(), velocityMeasurements[i]->position, velocityMeasurements[i]->posGaussFitterLin->fitMean - velocityMeasurements[i]->position);
        positionResidualsPoly2 -> SetPoint(positionResidualsPoly2->GetN(), velocityMeasurements[i]->position, velocityMeasurements[i]->posGaussFitterPoly2->fitMean - velocityMeasurements[i]->position);
        positionResidualsPoly3 -> SetPoint(positionResidualsPoly3->GetN(), velocityMeasurements[i]->position, velocityMeasurements[i]->posGaussFitterPoly2ThenFlat->fitMean - velocityMeasurements[i]->position);

        //fit the shoulders
        if(positions[i] < 21){
            TF1* shoulder = new TF1("shoulder", shoulderFun, 0, 100, 4);
            shoulder -> SetParameters(300, -6, 1, 5);
            //draw the fit function without erasing the previously fitted function
            velocityMeasurements[i]->correctedSpectrum->Fit(shoulder, "+Q", "", -15, -5);
            edgeDiffs.push_back(shoulder->GetParameter(1));
            edgeDiffErrs.push_back(shoulder->GetParError(1));
        }

        if(positions[i] > 21){
            TF1* shoulder = new TF1("shoulder", shoulderFun, 0, 100, 4);
            shoulder -> SetParameters(300, -3, 1, 5);
            //draw the fit function without erasing the previously fitted function
            velocityMeasurements[i]->correctedSpectrum->Fit(shoulder, "+Q", "", -15, 0);
            edgeDiffs.push_back(shoulder->GetParameter(1));
            edgeDiffErrs.push_back(shoulder->GetParError(1));
        }

        times.push_back(velocityMeasurements[i]->correctedGaussFitter->fitMean);
        timeErrs.push_back(velocityMeasurements[i]->correctedGaussFitter->fitMeanErr);
    }

    //add the sigmas to a multigraph
    reconstructedPositionSigmas = new TMultiGraph();
    positionSigmasLin->SetMarkerStyle(20);
    positionSigmasPoly2->SetMarkerStyle(21);
    positionSigmasPoly3->SetMarkerStyle(22);
    //set different colors
    positionSigmasLin->SetMarkerColor(kRed);
    positionSigmasPoly2->SetMarkerColor(kBlue);
    positionSigmasPoly3->SetMarkerColor(kGreen);
    //add to graph
    reconstructedPositionSigmas->Add(positionSigmasLin);
    reconstructedPositionSigmas->Add(positionSigmasPoly2);
    reconstructedPositionSigmas->Add(positionSigmasPoly3);

    reconstructedPositionResiduals = new TMultiGraph();
    positionResidualsLin->SetMarkerStyle(20);
    positionResidualsPoly2->SetMarkerStyle(21);
    positionResidualsPoly3->SetMarkerStyle(22);
    //set different colors
    positionResidualsLin -> SetMarkerColor(kRed);
    positionResidualsPoly2 -> SetMarkerColor(kBlue);
    positionResidualsPoly3 -> SetMarkerColor(kGreen);
    //add to graph
    reconstructedPositionResiduals->Add(positionResidualsLin);
    reconstructedPositionResiduals->Add(positionResidualsPoly2);
    reconstructedPositionResiduals->Add(positionResidualsPoly3);


    edgeDiffGraph = new TGraphErrors();
    for(int i = 0; i < edgeDiffs.size(); i++){
        edgeDiffGraph -> SetPoint(i,  positions[i],edgeDiffs[i]);
        edgeDiffGraph -> SetPointError(i, 0, edgeDiffErrs[i]);
    }
    //fit the edgeDiffGraph to a constant
    TF1* edgeDiffFit = new TF1("edgeDiffFit", "pol0", 0, 100);
    edgeDiffGraph -> Fit(edgeDiffFit, "RQ","", 0, 21);
    TF1* edgeDiffFit2 = new TF1("edgeDiffFit2", "pol0", 0, 100);
    edgeDiffGraph -> Fit(edgeDiffFit2, "RQ+","", 21, 55);
    double offset = edgeDiffFit2->GetParameter(0) - edgeDiffFit->GetParameter(0);
    vector<double> timeOffsets;
    timeOffsets.push_back(times[0]);
    for(int i = 1; i < times.size(); i++){
        times[i] = times[i]-times[0];
        if(positions[i] > 21){
            timeOffsets.push_back(times[0]+offset);
            cout << times[0]+offset << endl;
            times[i] = times[i] - offset;
        }
        else{
            timeOffsets.push_back(times[0]);
        }
        timeGraph -> SetPoint(timeGraph->GetN(), times[i],positions[i]);
        timeGraph -> SetPointError(timeGraph->GetN()-1, velocityMeasurements[i]->correctedGaussFitter->fitMeanErr, 0.2);
    }
    times[0] = 0; //we use the first measurement as the zeropoint.

    //write timeOffsets to a file
    ofstream timeOffsetFile;
    timeOffsetFile.open("output/timeOffsets.txt");
    for (int i = 0; i < timeOffsets.size(); ++i) {
        timeOffsetFile << timeOffsets[i] << endl;
    }

    TF1* flin = new TF1("lin", linear, -1, 100, 1);
    flin ->SetLineColor(kRed);
    timeGraph -> Fit(flin,"Q");
    TF1* fpoly = new TF1("2nd", poly2, -1, 100, 2);
    fpoly ->SetLineColor(kBlue);
    timeGraph -> Fit(fpoly,"Q+");
    TF1* fpoly3 = new TF1("poly3", poly3, -1, 100, 3);
    fpoly3 ->SetLineColor(kGreen);
    timeGraph -> Fit(fpoly3,"Q+");

    residualsLin = new TGraphErrors();
    residualsPoly2 = new TGraphErrors();
    residualsPoly2ThenFlat = new TGraphErrors();
    for(int i = 1; i < times.size(); i++){
        residualsLin -> SetPoint(residualsLin->GetN(), times[i], positions[i] - linear(&times[i], flin->GetParameters()));
        residualsLin -> SetPointError(residualsLin->GetN()-1, 0, timeErrs[i]);
        residualsPoly2 -> SetPoint(residualsPoly2->GetN(), times[i], positions[i] - poly2(&times[i], fpoly->GetParameters()));
        residualsPoly2 -> SetPointError(residualsPoly2->GetN()-1, 0, timeErrs[i]);
        residualsPoly2ThenFlat -> SetPoint(residualsPoly2ThenFlat->GetN(), times[i], positions[i] - poly3(&times[i], fpoly3->GetParameters()));
        residualsPoly2ThenFlat -> SetPointError(residualsPoly2ThenFlat->GetN()-1, 0, timeErrs[i]);
    }
    //do chi2 test
    double chi2Lin = 0;
    double chi2Poly2 = 0;
    double chi2Poly3 = 0;
    for(int i = 1; i < times.size(); i++){
        chi2Lin += pow((positions[i] - linear(&times[i], flin->GetParameters()))/0.2,2);
        chi2Poly2 += pow((positions[i] - poly2(&times[i], fpoly->GetParameters()))/0.2,2);
        chi2Poly3 += pow((positions[i] - poly3(&times[i], fpoly3->GetParameters()))/0.2,2);
    }
    //calculate p-value
    double pLin = TMath::Prob(chi2Lin, times.size()-1-1);
    double pPoly2 = TMath::Prob(chi2Poly2, times.size()-2-1);
    double pPoly3 = TMath::Prob(chi2Poly3, times.size()-3-1);
    cout << "chi2lin: "<< chi2Lin << " pLin: " << pLin << endl;
    cout << "chi2Poly2: "<< chi2Poly2 << " pPoly2: " << pPoly2 << endl;
    cout << "chi2Poly3: "<< chi2Poly3 << " pPoly3: " << pPoly3 << endl;

    for(int i = 1; i < times.size(); i++){
        //speedOfLightGraph -> SetPoint(speedOfLightGraph->GetN(), (positions[i]+positions[i-1])/2.0, 2.0*(positions[i] - positions[i-1])/(times[i] - times[i-1]));
        //speedOfLightGraph -> SetPointError(speedOfLightGraph->GetN()-1, (sqrt(2*0.2*0.2))/2.0, 2.0*(positions[i] - positions[i-1])/(times[i] - times[i-1])*sqrt(2*0.2*0.2/pow(positions[i] - positions[i-1],2) ));
        speedOfLightGraph -> SetPoint(speedOfLightGraph->GetN(), times[i], 2.0*positions[i]/times[i]);
        speedOfLightGraph -> SetPointError(speedOfLightGraph->GetN()-1, 0.2,0.2, 2.0*0.2/times[i],2.0*0.2/times[i]);
    }
    TF1 *funSolFit = new TF1("funSolFit", poly2wBase, 0, 100,3);
    funSolFit -> SetParameters(0.5, 0.5, 0.5);
    speedOfLightGraph -> Fit(funSolFit, "RQ");

    speedOfLightCanvas = new TMultiGraph();
    TGraph *linDerivGraph = new TGraph();
    TGraph *poly2DerivGraph = new TGraph();
    TGraph *poly2ThenFlatDerivGraph = new TGraph();
    for(int i = 1; i < times.size(); i++){
        linDerivGraph -> SetPoint(linDerivGraph->GetN(), positions[i], linearDerivative(&times[i], flin->GetParameters()));
        poly2DerivGraph -> SetPoint(poly2DerivGraph->GetN(), positions[i], poly2Derivative(&times[i], fpoly->GetParameters()));
        poly2ThenFlatDerivGraph -> SetPoint(poly2ThenFlatDerivGraph->GetN(), positions[i], poly3Derivative(&times[i], fpoly3->GetParameters()));
    }

    linDerivGraph -> SetLineColor(kRed);
    poly2DerivGraph -> SetLineColor(kBlue);
    poly2ThenFlatDerivGraph -> SetLineColor(kGreen);
    speedOfLightCanvas -> Add(speedOfLightGraph);
    speedOfLightCanvas -> Add(linDerivGraph);
    speedOfLightCanvas -> Add(poly2DerivGraph);
    speedOfLightCanvas -> Add(poly2ThenFlatDerivGraph);

    //save the parameters using a Positioner
    Positioner* positioner = new Positioner();
    positioner -> setParams({fpoly3 -> GetParameter(0), fpoly3 -> GetParameter(1), fpoly3 -> GetParameter(2)});
    positioner -> saveParams("output/positionParamsPoly2ThenFlat.txt");
    positioner ->setParams({flin -> GetParameter(0)});
    positioner -> saveParams("output/positionParamsLin.txt");
    positioner ->setParams({fpoly -> GetParameter(0), fpoly -> GetParameter(1)});
    positioner -> saveParams("output/positionParamsPoly2.txt");
    positioner ->setParams({funSolFit -> GetParameter(0), funSolFit -> GetParameter(1), funSolFit -> GetParameter(2)});
    positioner -> saveParams("output/speedOfLightParams.txt");


    //add the two graphs to residualGraph with different markerstyles
    residualGraph = new TMultiGraph();
    residualsLin->SetMarkerStyle(20);
    residualsPoly2->SetMarkerStyle(21);
    residualsPoly2ThenFlat->SetMarkerStyle(22);
    //set different colors
    residualsLin->SetMarkerColor(kRed);
    residualsPoly2->SetMarkerColor(kBlue);
    residualsPoly2ThenFlat->SetMarkerColor(kGreen);

    residualGraph->Add(residualsLin);
    residualGraph->Add(residualsPoly2);
    residualGraph->Add(residualsPoly2ThenFlat);


    //add the two graphs to allSigmas with different markerstyles
    correctedSigmas->SetMarkerStyle(20);
    uncorrectedSigmas->SetMarkerStyle(21);
    //set different colors
    correctedSigmas->SetMarkerColor(kRed);
    uncorrectedSigmas->SetMarkerColor(kBlue);
    allSigmas->Add(correctedSigmas);
    allSigmas->Add(uncorrectedSigmas);
}

void MultiVelocity::saveHistograms(TString fileName) {
    TFile* file = new TFile(fileName, "RECREATE");
    for (int i = 0; i < velocityMeasurements.size(); ++i) {
        auto newdir = file ->mkdir(Form("run%i", runNumbers[i]));
        newdir -> cd();
        velocityMeasurements[i]->correctedSpectrum->Write();
        velocityMeasurements[i]->uncorrectedSpectrum->Write();
        velocityMeasurements[i]->posSpectrumLin->Write();
        velocityMeasurements[i]->posSpectrumPoly2->Write();
        velocityMeasurements[i]->posSpectrumPoly2ThenFlat->Write();
        velocityMeasurements[i]->correctedSumSpectrum->Write();
        //create canvas with both histograms in different colors simultaneously
        auto c = new TCanvas(Form("c%i", runNumbers[i]), Form("c%i", runNumbers[i]));
        c->cd();
        velocityMeasurements[i]->correctedSpectrum->SetLineColor(kRed);
        velocityMeasurements[i]->uncorrectedSpectrum->SetLineColor(kBlue);
        velocityMeasurements[i]->correctedSpectrum->Draw();
        velocityMeasurements[i]->uncorrectedSpectrum->Draw("SAME");
        c->Write();
    }
    file->cd();
    allSigmas->Write("allSigmas");
    correctedSigmas -> Write("correctedSigmas");
    uncorrectedSigmas -> Write("uncorrectedSigmas");
    speedOfLightGraph -> Write("speedOfLightGraph");
    timeGraph -> Write("timeGraph");
    residualGraph ->Write("residualGraph");
    edgeDiffGraph -> Write("edgeDiffGraph");
    speedOfLightCanvas ->Write();
    reconstructedPositionResiduals -> Write("reconstructedPositionResiduals");
    reconstructedPositionSigmas -> Write("reconstructedPositionSigmas");
    file->Close();
}
//
// Created by jeppe on 11/12/23.
//

#include "Positioner.hh"
void Positioner::setParams(vector<double> params){
    this->params = params;
}

void Positioner::saveParams(TString fileName){
    //save the parameters to a txt file
    ofstream file;
    file.open(fileName);
    for (int i = 0; i < params.size(); ++i) {
        file << params[i] << endl;
    }
}

void Positioner::loadParams(TString fileName){
    //load the parameters from a txt file
    ifstream file;
    file.open(fileName);
    double param;
    while(file >> param){
        params.push_back(param);
    }
}

void Positioner::loadSpeedParams(TString fileName){
    //load the parameters from a txt file
    ifstream file;
    file.open(fileName);
    double param;
    while(file >> param){
        speeds.push_back(param);
    }
}

void Positioner::loadSigmaParams(TString fileName){
    //load the parameters from a txt file
    ifstream file;
    file.open(fileName);
    double param;
    while(file >> param){
        sigmas.push_back(param);
    }
}

double Positioner::getSpeed(int bar){
    return speeds[bar];
}

double Positioner::getSigma(int bar){
    return sigmas[bar];
}

double Positioner::getVelocity(double tDiff) {
    tDiff = abs(tDiff);
    /*if(tDiff > 8){ //8 ns is maximal time difference - when the source is at the edge of the detector
        return params[0] + params[1]*8 + params[2]*8*8;
    }*/
    return params[0] + params[1]*tDiff + params[2]*tDiff*tDiff;
}

double Positioner::getDelay(double tDiff) {
    return 120/(2.0*getVelocity(tDiff));
}

double Positioner::getPosition(double tDiff){
    //return the position from the time difference
    return lin(tDiff, params.data());
}

double Positioner::lin(double tDiff, double *params){
    //linear function
    return params[0]*tDiff;
}

double Positioner::poly2(double tDiff, double *params){
    bool otherSide = false;
    if(tDiff < 0){
        tDiff = -tDiff;
        otherSide = true;
    }

    //second order polynomial
    if(otherSide){
        return -(params[0]*tDiff + params[1]*tDiff*tDiff);
    }
    else{
        return params[0]*tDiff + params[1]*tDiff*tDiff;
    }
}

double Positioner::poly3(double tDiff, double *params){
    bool otherSide = false;
    if(tDiff < 0){
        tDiff = -tDiff;
        otherSide = true;
    }
    if(otherSide){
        return -(params[0]*tDiff + params[1]*tDiff*tDiff + params[2]*tDiff*tDiff*tDiff);
    }
    else{
        return +(params[0]*tDiff + params[1]*tDiff*tDiff + params[2]*tDiff*tDiff*tDiff);
    }
}
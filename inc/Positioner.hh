//
// Created by jeppe on 11/12/23.
//

#ifndef INDIETESTS_POSITIONER_HH
#define INDIETESTS_POSITIONER_HH

#include <vector>
#include <TString.h>
#include "fstream"

using namespace std;

class Positioner {
public:
    vector<double> params;

    Positioner(){}

    void setParams(vector<double> params);

    void saveParams(TString fileName);

    void loadParams(TString fileName);

    double getPosition(double tDiff);

    double getVelocity(double tDiff);

    double getDelay(double tDiff);

    static double lin(double tDiff, double *params);

    static double poly2(double tDiff, double *params);

    static double poly3(double tDiff, double *params);
};

#endif //INDIETESTS_POSITIONER_HH

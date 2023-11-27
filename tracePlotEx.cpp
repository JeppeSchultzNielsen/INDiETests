//
// Created by jeppe on 11/12/23.
//
#include "MultiTracePlotter.hh"
#include "vector"

using namespace std;

int main(){
    vector<int> runNumbers = {2, 3,4,5,6,7,8,9,10,11,12,14};
    vector<double> positions = {0, 5, 10, 15, 20, 25, 30, 35 ,40, 45, 50, 55};

    auto mtp = new MultiTracePlotter(runNumbers, positions, "indieTestsTraces/Run%03d.root");
    mtp -> doAnalyses();
    mtp -> saveHistograms("output/traceShapesVar.root");

    return 0;

}
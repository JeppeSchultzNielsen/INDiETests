//
// Created by jeppe on 10/23/23.
//

#include "attenuation.hh"
#include "multiAttenuation.h"
#include "attenuationPositioner.h"
#include "WalkCorrector.h"

int main(){
    /*vector<int> runNumbers = {2,3,4,5,6,7,8,9,10,11,12,14};
    vector<double> positions = {0, 5, 10, 15, 20, 25, 30, 35 ,40, 45, 50, 55};
    auto ma = new MultiAttenuation(runNumbers, positions, "UnpackedINDiETests/Run%03d.root", "output/attn_%d.root");
    ma -> doAnalyses();
    ma -> createMultiGraph();
    ma -> saveMultiGraph("output/multiGraph.root");*/

    /*auto ap = new AttenuationPositioner();
    ap -> loadParams("output/attenuationParams.txt");
    ap ->attenuationTest("UnpackedINDiETests/Run014.root", "output/centerPositionTest.root");*/

    auto wc = new WalkCorrector();
    wc ->loopOverFile("UnpackedINDiETests/Run012.root");
    wc -> fitWalkTH1s();
    wc ->saveHistograms("output/walkCorrecter.root");

}
#include "timeWriter.hh"

void TimeWriter::writeTime(int current_block, int full_size) {

    if(current_block % printEvery != 0)
        return;

    double elapsed=0, remaining=0; //in seconds
    int remaining_min, remaining_sec;

    elapsed = ((double)clock() - start_clock) / CLOCKS_PER_SEC;

    remaining = (full_size - current_block)*elapsed/current_block;

    remaining_min = (int)remaining / 60 ;
    remaining_sec = (int)remaining % 60 ;

    //printf(" [ %d%% - %2dm%2ds ]", 100*float(current_block)/float(full_size), remaining_min, remaining_sec);
    //printf(" [ %d MB/s - %2dm%2ds ]", (float(current_block)/1048576)/float(elapsed), remaining_min, remaining_sec);
    cout << Form(" [ %2dm%2ds remaining ]", remaining_min, remaining_sec) << flush;
    cout << "\r" << (double) (100.0 * current_block / (1.0 * full_size)) << "%";
}
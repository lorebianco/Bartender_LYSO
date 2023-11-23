#ifndef EVENT_HH
#define EVENT_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <cfloat>

#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TMath.h>
#include <TString.h>
#include <TSystem.h>
#include <TTree.h>
#include <TRandom3.h>
#include <TCanvas.h>

#include "globals.hh"
#include "sample.hh"


class Event
{
public:
    Event();
    ~Event();

    Sample fFront[CHANNELS];
    Sample fBack[CHANNELS];

    void SetFrontChannel(Int_t channel, TF1 *FinalWave);
    void SetBackChannel(Int_t channel, TF1 *FinalWave);
    void DrawFrontChannel(Int_t channel);
    void DrawBackChannel(Int_t channel);
    void SaveEventData(Int_t eventNum, const char* outputFilename);
};

#endif
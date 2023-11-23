#ifndef SAMPLE_HH
#define SAMPLE_HH

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
#include "globals.hh"


class Sample
{
public:
    Sample();
    ~Sample();

    void SetSamples(TF1 *FinalWave);
    void DrawSampling();
    
    Double_t fBins[SAMPLINGS];
    Double_t fSamples[SAMPLINGS];
};

#endif
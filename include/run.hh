#ifndef RUN_HH
#define RUN_HH

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
#include "functions.hh"


class Run
{
public:
    Run(Int_t events);
    ~Run();

    Int_t EVENTS;

    Double_t*** fFront;
    Double_t*** fBack;

    //Int_t fFrontPhNumber[EVENTS][CHANNELS];
    //Int_t fBackPhNumber[EVENTS][CHANNELS];

    void InitializeFrontWaveforms();
    void InitializeBackWaveforms();

    void SetFrontWaveform(Int_t event, Int_t channel, Double_t A, Double_t tau_rise, Double_t tau_dec, Double_t start);
    void SetBackWaveform(Int_t event, Int_t channel, Double_t A, Double_t tau_rise, Double_t tau_dec, Double_t start);

    //void SetFrontPhotonsNumber(Int_t event, Int_t channel, Int_t phNumber) {fFrontPhNumber[channel] = phNumber; }
    //void SetBackPhotonsNumber(Int_t event, Int_t channel, Int_t phNumber) {fBackPhNumber[channel] = phNumber; };
    
    void DrawFront(Int_t event, Int_t channel);
    void DrawBack(Int_t event, Int_t channel);
    
    //Int_t GetFrontPhotonsNumber(Int_t event, Int_t channel) { return fFrontPhNumber[event][channel]; }
    //Int_t GetBackPhotonsNumber(Int_t event, Int_t channel) { return fBackPhNumber[event][channel]; }

    void SaveRun(const char* outputFilename);
};

#endif
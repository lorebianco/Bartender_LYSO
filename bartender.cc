//****************************************************************************//
//                        Lorenzo Bianco 13/11/2023                           //
//                                                                            //
//             My 'Bartender' for the LYSO calorimeter prototype              //
//                                                                            //
//****************************************************************************//

/** 
 * @file bartender.cc
 * @brief main file of the project.
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <cfloat>
#include <chrono>

#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH3D.h>
#include <TMath.h>
#include <TString.h>
#include <TSystem.h>
#include <TTree.h>
#include <TRandom3.h>
#include <TCanvas.h>

#include "globals.hh"
#include "configure.hh"
#include "summary.hh"
#include "bar.hh"
#include "SiPM.hh"

using namespace std;
using namespace TMath;



int main(int argc, char** argv)
{
    cout << "Start" << endl;

    const char *mcFilename = argv[1];
    const char *sipmFilename = argv[2];
    
    SiPM *sipm = new SiPM();
    Bar *bar = new Bar(mcFilename);
    Bartender_Configure(sipmFilename, bar, sipm);
    
    bar->SetParsDistro();


    TFile *mcFile = TFile::Open(mcFilename, "READ");
    TTree *tFront = mcFile->Get<TTree>("F");
    TTree *tBack = mcFile->Get<TTree>("B");
    TTree *tCry = mcFile->Get<TTree>("Cry");

    tFront->SetBranchStatus("*", false);
    tBack->SetBranchStatus("*", false);
    tCry->SetBranchStatus("*", false);

    tFront->SetBranchStatus("fEvent", true);
    tFront->SetBranchStatus("fT", true);
    tFront->SetBranchStatus("fChannel", true);
    tBack->SetBranchStatus("fEvent", true);
    tBack->SetBranchStatus("fT", true);
    tBack->SetBranchStatus("fChannel", true);
    tCry->SetBranchStatus("fEvent", true);
    tCry->SetBranchStatus("fTin", true);


    Int_t fEvent_front, fEvent_back, fChannel_front, fChannel_back;
    Double_t fT_front, fT_back;

    tFront->SetBranchAddress("fEvent", &fEvent_front);
    tBack->SetBranchAddress("fEvent", &fEvent_back);

    tFront->SetBranchAddress("fChannel", &fChannel_front);
    tBack->SetBranchAddress("fChannel", &fChannel_back);

    tFront->SetBranchAddress("fT", &fT_front);
    tBack->SetBranchAddress("fT", &fT_back);

    if(!(tFront->GetMaximum("fEvent") == tBack->GetMaximum("fEvent") && tBack->GetMaximum("fEvent") == tCry->GetMaximum("fEvent"))) 
    {
        cerr << "Error: something wrong in the MC file happened: events in trees are not the same" << endl;
        return 1;
    }

    cout << "Trees loaded. Now there will be the Bartender" << endl;


    Int_t fNumberofEvents = tCry->GetMaximum("fEvent") + 1;
    bar->InitializeBaselines(fNumberofEvents);

    auto start_chrono = chrono::high_resolution_clock::now();
    Int_t evIdx;
    for(Int_t k = 0; k < tFront->GetEntries(); k++)
    {
        tFront->GetEntry(k);
        
        bar->SetFrontWaveform(fEvent_front, fChannel_front, fT_front);

        if(k == 0) evIdx = fEvent_front;
        if(k!= 0 && evIdx != fEvent_front)
        {
            cout << "Processed Front Event " << evIdx << endl;
            evIdx = fEvent_front;
        }
    }
    cout << "Processed Front Event " << evIdx << endl;
    cout << "Done Front Detector" << endl;


    for(Int_t k = 0; k < tBack->GetEntries(); k++)
    {
        tBack->GetEntry(k);
        
        bar->SetBackWaveform(fEvent_back, fChannel_back, fT_back);

        if(k == 0) evIdx = fEvent_back;
        if(k!= 0 && evIdx != fEvent_back)
        {
            cout << "Processed Back Event " << evIdx << endl;
            evIdx = fEvent_back;
        }
    }
    cout << "Processed Back Event " << evIdx << endl;
    cout << "Done Back Detector" << endl;

    bar->SaveBar();

    auto end_chrono = chrono::high_resolution_clock::now();
    chrono::duration<Double_t> duration = end_chrono - start_chrono;

    Bartender_Summary(duration.count(), bar, sipm);


    mcFile->Close();
    delete sipm;
    delete bar;

    return 0;
} 
////////////////////////////////////////////////////////////////////////////////
//                        Lorenzo Bianco 13/11/2023                           //
//                                                                            //
//             My 'Bartender' for the LYSO calorimeter prototype              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

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
#include "run.hh"

using namespace std;
using namespace TMath;

TH3D *hAll;
TRandom3 *randAll = new TRandom3(0);
TRandom3 *randNoise = new TRandom3(0);


int main(int argc, char** argv)
{
    cout << "Start" << endl;

    const char *inputFilename = argv[1];
    
    if(hAll==nullptr)
    {
        SetDistros("/home/lorenzo/MEG_Project/Acquisizioni/Studio_SIPM/Preamp_Ext/Dati19_10/Dati_root/dati_spectrum_T20_V5478_1pe_fit_params.txt", 0, 1.5);
    }

    cout << "Distro setted" << endl;

    TFile *mcFile = TFile::Open(inputFilename, "READ");
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

    Int_t fNumberofEvents = tFront->GetMaximum("fEvent") + 1;
    Run *run = new Run(fNumberofEvents);

    Int_t evIdx;
    for(Int_t k = 0; k < tFront->GetEntries(); k++)
    {
        tFront->GetEntry(k);

        Double_t A, tau_rise, tau_dec;
        hAll->GetRandom3(A, tau_rise, tau_dec, randAll);
        
        run->SetFrontWaveform(fEvent_front, fChannel_front, A, tau_rise, tau_dec, fT_front);

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

        Double_t A, tau_rise, tau_dec;
        hAll->GetRandom3(A, tau_rise, tau_dec, randAll);
        
        run->SetBackWaveform(fEvent_back, fChannel_back, A, tau_rise, tau_dec, fT_back);

        if(k == 0) evIdx = fEvent_back;
        if(k!= 0 && evIdx != fEvent_back)
        {
            cout << "Processed Back Event " << evIdx << endl;
            evIdx = fEvent_back;
        }
    }
    cout << "Processed Back Event " << evIdx << endl;
    cout << "Done Back Detector" << endl;

    run->SaveRun(inputFilename);

    mcFile->Close();

    delete run;
    delete randAll, randNoise;
    delete hAll;

    return 0;
} 
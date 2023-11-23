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
#include "event.hh"
#include "sample.hh"

using namespace std;
using namespace TMath;




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

    Int_t fNumberofEvents = tFront->GetMaximum("fEvent") + 1;
    vector<Int_t> fTimesFront[fNumberofEvents][CHANNELS];
    vector<Int_t> fTimesBack[fNumberofEvents][CHANNELS];

    cout << "Trees loaded. Now it will be assignment" << endl;

    for(Int_t k = 0; k < tFront->GetEntries(); k++)
    {
        tFront->GetEntry(k);
        fTimesFront[fEvent_front][fChannel_front].push_back(Nint(fT_front+ZERO_TIME_BIT));
    }

    for(Int_t k = 0; k < tBack->GetEntries(); k++)
    {
        tBack->GetEntry(k);
        fTimesBack[fEvent_back][fChannel_back].push_back(Nint(fT_back+ZERO_TIME_BIT));
    }
    
    cout << "Assignment done" << endl;

    delete tFront;
    delete tBack;
    delete tCry;
    mcFile->Close();

    cout << "Start of Bartender for full run" << endl;

    Event *event = new Event();

    for (Int_t i = 0; i < 5; i++)
    {
        for (Int_t j = 0; j < CHANNELS; j++)
        {
            Int_t N_phel_front = fTimesFront[i][j].size();
            Int_t N_phel_back = fTimesBack[i][j].size();

            event->SetFrontChannel(j, Bartender(N_phel_front, fTimesFront[i][j]));
            event->SetBackChannel(j, Bartender(N_phel_back, fTimesBack[i][j]));
        }

        //event->DrawFrontChannel(80);

        //event->SaveEventData(i, "output.txt");
        cout << "Processed event number " << i << endl;
    }


    delete randAll, randNoise;
    delete hAll;
    delete event;

    return 0;
} 
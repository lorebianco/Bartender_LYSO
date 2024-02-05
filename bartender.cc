//****************************************************************************//
//                        Lorenzo Bianco 13/11/2023                           //
//                                                                            //
//             My 'Bartender' for the LYSO calorimeter prototype              //
//                                                                            //
//****************************************************************************//

/**
 * @file bartender.cc
 * @brief Definition of the main function of Bartender_LYSO.
 */
#include <iostream>
#include <vector>
#include <chrono>
 
#include <TFile.h>
#include <TTree.h>
 
#include "globals.hh"
#include "configure.hh"
#include "summary.hh"
#include "bar.hh"
#include "SiPM.hh"
 
using namespace std;

/**
 * @brief Main of the application.
 *
 * It manages the simulation flow. To begin with, it creates instances of the
 * Bar and SiPM classes and invokes Bartender_Configure() and
 * Bar::SetParsDistro() method. Next, it accesses the "lyso" TTree from the
 * Monte Carlo simulation.
 * After initializing all the waveform containers through
 * Bar::InitializeBaselines(), it initiates the core of the simulation. By
 * looping through all events and hit collections in the TTree, it constructs
 * the waveforms by calling Bar::SetFrontWaveforms() and
 * Bar::SetBackWaveform(). Finally, it saves all the data using Bar::SaveBar()
 * and invokes Bartender_Summary(). 
 */
int main(int argc, char** argv)
{
    cout << "Start" << endl;

    // Get input files
    const char *mcFilename = argv[1];
    const char *sipmFilename = argv[2];

    // Intantiates and configure SiPM and Bar members
    SiPM *sipm = new SiPM();
    Bar *bar = new Bar(mcFilename);
    Bartender_Configure(sipmFilename, bar, sipm);

    // Set the parameters histogram
    bar->SetParsDistro();

    // Access the "lyso" TTree and its useful branches
    unique_ptr<TFile> mcFile(TFile::Open(mcFilename, "READ"));
    TTree *lyso = mcFile->Get<TTree>("lyso");

    lyso->SetBranchStatus("*", false);

    lyso->SetBranchStatus("Event", true);
    lyso->SetBranchStatus("NHits_F", true);
    lyso->SetBranchStatus("NHits_B", true);    
    lyso->SetBranchStatus("T_F", true);
    lyso->SetBranchStatus("Ch_F", true);
    lyso->SetBranchStatus("T_B", true);
    lyso->SetBranchStatus("Ch_B", true);

    int fEvent;
    int fNHits_F, fNHits_B;
    vector<double> *fT_F = 0, *fT_B = 0;
    vector<int> *fCh_F = 0, *fCh_B = 0;

    lyso->SetBranchAddress("Event", &fEvent);
    lyso->SetBranchAddress("NHits_F", &fNHits_F);
    lyso->SetBranchAddress("NHits_B", &fNHits_B);
    lyso->SetBranchAddress("Ch_F", &fCh_F);
    lyso->SetBranchAddress("Ch_B", &fCh_B);
    lyso->SetBranchAddress("T_F", &fT_F);
    lyso->SetBranchAddress("T_B", &fT_B);

    cout << "Trees loaded. Now there will be the Bartender" << endl;

    // Get the number of events and initialize WF containers
    Int_t fNumberofEvents = lyso->GetEntries();
    bar->InitializeBaselines(fNumberofEvents);

    // Start with the Bartender
    auto start_chrono = chrono::high_resolution_clock::now();

    for(Int_t k = 0; k < fNumberofEvents; k++)
    {
        lyso->GetEntry(k);

        Int_t *fCh_F_data = fCh_F->data();
        Int_t *fCh_B_data = fCh_B->data();
        Double_t *fT_F_data =  fT_F->data();
        Double_t *fT_B_data =  fT_B->data();

        for(Int_t j = 0; j < fNHits_F; j++)
        {
            bar->SetFrontWaveform(fEvent, fCh_F_data[j], fT_F_data[j]);
        }
        for(Int_t j = 0; j < fNHits_B; j++)
        {
            bar->SetBackWaveform(fEvent, fCh_B_data[j], fT_B_data[j]);
        }

        if(k%(fNumberofEvents/10) == 0) cout << "Processed " << k << " events" << endl;
    }

    // Save data
    bar->SaveBar();

    auto end_chrono = chrono::high_resolution_clock::now();
    chrono::duration<Double_t> duration = end_chrono - start_chrono;

    // Save summary
    Bartender_Summary(duration.count(), bar, sipm);

    // Free the memory
    lyso->ResetBranchAddresses();
    delete fT_F, fT_B, fCh_F, fCh_B;
    delete sipm;
    delete bar;

    // Finally
    return 0;
} 
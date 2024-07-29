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
    // Get input files
    const char *mcFilename = argv[1];
    const char *sipmFilename = argv[2];
    Int_t threadID = 0;
    if(argc == 4)
        threadID = std::stoi(argv[3]);

    cout << "BarWT" << threadID << ">> Start" << endl;
    
    // Intantiates and configure SiPM and Bar members
    SiPM *sipm = new SiPM();
    Bar *bar = new Bar(mcFilename, threadID);
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

    Int_t fEvent;
    Int_t fNHits_F, fNHits_B;
    vector<Double_t> *fT_F = 0, *fT_B = 0;
    vector<Int_t> *fCh_F = 0, *fCh_B = 0;

    lyso->SetBranchAddress("Event", &fEvent);
    lyso->SetBranchAddress("NHits_F", &fNHits_F);
    lyso->SetBranchAddress("NHits_B", &fNHits_B);
    lyso->SetBranchAddress("Ch_F", &fCh_F);
    lyso->SetBranchAddress("Ch_B", &fCh_B);
    lyso->SetBranchAddress("T_F", &fT_F);
    lyso->SetBranchAddress("T_B", &fT_B);

    cout << "BarWT" << threadID << ">> Trees loaded. Now there will be the Bartender" << endl;

    // Get the number of events and initialize WF containers
    Int_t nEntries = lyso->GetEntries();
    bar->SetEvents(nEntries);

    // Start with the Bartender
    auto start_chrono = chrono::high_resolution_clock::now();

    for(Int_t k = 0; k < nEntries; k++)
    {
        lyso->GetEntry(k);

        Int_t *fCh_F_data = fCh_F->data();
        Int_t *fCh_B_data = fCh_B->data();
        Double_t *fT_F_data =  fT_F->data();
        Double_t *fT_B_data =  fT_B->data();

        bar->InitializeBaselines(fEvent);

        for(Int_t j = 0; j < fNHits_F; j++)
        {
            bar->SetFrontWaveform(fCh_F_data[j], fT_F_data[j]);
        }
        for(Int_t j = 0; j < fNHits_B; j++)
        {
            bar->SetBackWaveform(fCh_B_data[j], fT_B_data[j]);
        }
        
        bar->SaveEvent();
        bar->ClearContainers();
        
        // Print progress
        if(nEntries < 10 || k % (nEntries / 10) == 0)
            cout << "BarWT" << threadID << ">> Processed " << k + 1 << " events" << endl;
    }

    // Save data
    bar->SaveBar();

    auto end_chrono = chrono::high_resolution_clock::now();
    chrono::duration<Double_t> duration = end_chrono - start_chrono;

    // Free the memory
    lyso->ResetBranchAddresses();
    delete fT_F, fT_B, fCh_F, fCh_B;
    delete sipm;
    delete bar;

    // Finally
    return 0;
} 
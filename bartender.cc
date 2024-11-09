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
#include "summary.hh"

 
using namespace std;

/**
 * @brief Main of the application.
 *
 * It manages the simulation flow. To begin with, it creates instances of the
 * BarLYSO and SiPM classes and invokes Bartender_Configure() and
 * BarLYSO::SetParsDistro() method. Next, it accesses the "lyso" TTree from the
 * Monte Carlo simulation.
 * After initializing all the waveform containers through
 * BarLYSO::InitializeBaselines(), it initiates the core of the simulation. By
 * looping through all events and hit collections in the TTree, it constructs
 * the waveforms by calling BarLYSO::SetFrontWaveforms() and
 * BarLYSO::SetBackWaveform(). Finally, it saves all the data using BarLYSO::SaveBar()
 * and invokes Bartender_Summary(). 
 */
int main(int argc, char** argv)
{
    // Get input files
    const char *mcFilename = argv[1];
    const char *sipmFilename = argv[2];
    Int_t threadID = -1;
    bool isMultithreading = false;

    if(argc == 4)
    {
        threadID = std::stoi(argv[3]);
        isMultithreading = true;
    }

    if(!isMultithreading)
        cout << "BarST>> Start" << endl;
    else
        cout << "BarWT" << threadID << ">> Start" << endl; 

    // Instances and configuration of SiPM and BarLYSO
    SiPM *sipm = new SiPM();
    BarLYSO *bar = new BarLYSO(mcFilename, threadID);
    Bartender_Configure(sipmFilename, bar, sipm);

    // Set parameters and load TTree
    bar->SetParsDistro();
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

    // Number of events and initialize containers
    Int_t nEntries = lyso->GetEntries();
    bar->SetEvents(nEntries);
    
    if(!isMultithreading)
        cout << "BarST>> Trees loaded. Starting Bartender for " << nEntries << " events" << endl;
    else
        cout << "BarWT" << threadID << ">> Trees loaded. Starting Bartender for " << nEntries << " events" << endl;

    // Start with the Bartender
    auto start_chrono = chrono::high_resolution_clock::now();

    // Sampling times
    bar->SetSamplingTimes();

    // Event loop
    for(Int_t k = 0; k < nEntries; k++)
    {
        lyso->GetEntry(k);

        bar->InitializeBaselines(fEvent);

        for(Int_t j = 0; j < fNHits_F; j++)
            bar->SetFrontWaveform(fCh_F->data()[j], fT_F->data()[j]);

        for(Int_t j = 0; j < fNHits_B; j++)
            bar->SetBackWaveform(fCh_B->data()[j], fT_B->data()[j]);

        bar->SaveEvent();
        bar->ClearContainers();


        if(nEntries < 10 || k % (nEntries / 10) == 0)
        {
            if(!isMultithreading)
                cout << "\rBarST>> Processed " << k + 1 << " events" << flush;
            else
                cout << "\rBarWT" << threadID << ">> Processed " << k + 1 << " events" << flush;
        }
    }

    // Save data
    bar->SaveBar();

    auto end_chrono = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_chrono - start_chrono;

    // Single-thread summary
    if(!isMultithreading)
    {
        Bartender_Summary(sipmFilename, bar->GetID(), duration.count());
    }

    // Free memory
    lyso->ResetBranchAddresses();
    delete fT_F, fT_B, fCh_F, fCh_B;
    delete sipm;
    delete bar;

    // Finally
    return 0;
}
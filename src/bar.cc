/** 
 * @file bar.cc
 * @brief Definition of the class Bar
 */
#include "bar.hh"

using namespace std;
using namespace TMath;


Bar::Bar(const char*inputFilename)
{
    // Regular way to extract digits from the filename
    regex regex("\\d+");
    smatch match;
    string filename = inputFilename;
    
    // Check if there is a match for digits in the filename
    if(regex_search(filename, match, regex))
    {
        // Extract the matched digits as a string
        string runID = match.str();
        if(!runID.empty())
        {
            // Convert the extracted string to an integer and assign it to the fID member variable
            fID = stoi(runID);
        }
    }
}



Bar::~Bar()
{
    // Delete the WFs containers
    for(Int_t i = 0; i < EVENTS; i++)
    {
        for(Int_t j = 0; j < CHANNELS; j++)
        {
            delete[] fFront[i][j];
            delete[] fBack[i][j];
        }
        delete[] fFront[i];
        delete[] fBack[i];
    }
    delete[] fFront;
    delete[] fBack;

    // Delete hPars and TRandom3 objs
    delete hPars;
    delete fRandPars, fRandNoise;
}



void Bar::SetParsDistro()
{
    Int_t status;
    Double_t my_charge, A, tau_rise, tau_dec;
    
    // Get a TTree from the text file of parameters
    TTree *tree = new TTree("tree", "mytree");
    tree->ReadFile(fInputFilename.c_str());
 
    tree->SetBranchAddress("Status", &status);
    tree->SetBranchAddress("my_charge", &my_charge);
    tree->SetBranchAddress("A", &A);
    tree->SetBranchAddress("Tau_rise", &tau_rise);
    tree->SetBranchAddress("Tau_dec", &tau_dec);
    
    // Create hPars
    hPars = new TH3D("hPars", "Fitted All", fHisto_A[0], fHisto_A[1], fHisto_A[2], fHisto_Tau_rise[0], fHisto_Tau_rise[1], fHisto_Tau_rise[2], fHisto_Tau_dec[0], fHisto_Tau_dec[1], fHisto_Tau_dec[2]);
    
    // Fill hPars
    for(Int_t i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);

        // Check conditions about charge
        if(status==0 && my_charge >= fChargeCuts[0] && my_charge <= fChargeCuts[1])       
        {
            hPars->Fill(A, tau_rise, tau_dec);
        }
    }
 
    cout << "Distro setted with entries with charge in [" << fChargeCuts[0] << ", " << fChargeCuts[1] << "]" << endl;

    // Delete the TTree
    delete tree;
}



Double_t Bar::Wave_OnePhel(Double_t t, Double_t A, Double_t tau_rise, Double_t tau_dec, Int_t timePhel)
{
    Double_t funcVal;
    Double_t expRise = Exp(-(t-timePhel)/tau_rise);
    Double_t expDec = Exp(-(t-timePhel)/tau_dec);

    // It happens sometimes when t << timePhel or t >> timePhel, so it's just a numerical fixing
    if(expRise > DBL_MAX || expRise < DBL_MIN) expRise = 0;
    if(expDec > DBL_MAX || expDec < DBL_MIN) expDec = 0;

    // 1-Phel function
    funcVal = (-A*(expDec-expRise))*((t > timePhel) ? 1:0);

    return funcVal;
}



void Bar::InitializeBaselines(Int_t newEvents)
{
    // Set number of events in the run
    EVENTS = newEvents;
    
    // Initialize whole WF containers [EVENTS]x[CHANNELS]x[SAMPLINGS]
    fFront = new Double_t**[EVENTS];
    fBack = new Double_t**[EVENTS];
 
    for(Int_t ev = 0; ev < EVENTS; ev++)
    {
        fFront[ev] = new Double_t*[CHANNELS];
        fBack[ev] = new Double_t*[CHANNELS];
        
        for(Int_t ch = 0; ch < CHANNELS; ch++)
        {
            fFront[ev][ch] = new Double_t[SAMPLINGS];
            fBack[ev][ch] = new Double_t[SAMPLINGS];
 
            for(Int_t bin = 0; bin < SAMPLINGS; bin++)
            {
                // Set every bin with gaussian noise
                fFront[ev][ch][bin] = Add_Noise();
                fBack[ev][ch][bin] = Add_Noise();
            }
        }
    }
}



void Bar::SetFrontWaveform(Int_t event, Int_t channel, Double_t start)
{
    // Sample from hPars the parameters of 1-Phel WF
    Double_t A, tau_rise, tau_dec;
    hPars->GetRandom3(A, tau_rise, tau_dec, fRandPars);
    
    // Evaluate and sum the new 1-Phel WF to the existing one
    for(Int_t bin = 0; bin < SAMPLINGS; bin++)
    {
        fFront[event][channel][bin] += Wave_OnePhel(bin, A, tau_rise, tau_dec, Nint(start+ZERO_TIME_BIN));
    }
}



void Bar::SetBackWaveform(Int_t event, Int_t channel, Double_t start)
{
    // Sample from hPars the parameters of 1-Phel WF
    Double_t A, tau_rise, tau_dec;
    hPars->GetRandom3(A, tau_rise, tau_dec, fRandPars);
    
    // Evaluate and sum the new 1-Phel WF to the existing one
    for(Int_t bin = 0; bin < SAMPLINGS; bin++)
    {
        fBack[event][channel][bin] += Wave_OnePhel(bin, A, tau_rise, tau_dec, Nint(start+ZERO_TIME_BIN));
    }
}



void Bar::SaveBar()
{   
    // Determine the output filename based on the Bar ID
    string outputFilename;
    if(fID)
    {
        outputFilename = "BarID_" + to_string(fID) + ".txt";
    }
    else outputFilename = "output.txt";
 
    // Open the output file for writing, truncating it if it already exists
    ofstream outputFile(outputFilename, ofstream::out | ofstream::trunc);
    
    // Check if the output file is successfully opened
    if(!outputFile.is_open())
    {
        cout << "Can't open output file!" << endl;
        return;
    }

    // Loop through each event and save data to the output file
    for(Int_t ev = 0; ev < EVENTS; ev++)
    {
        // Write event header
        outputFile << "#Event " << ev << endl;
        outputFile << "#" << endl;
        outputFile << "#Front Detector" << endl;
        outputFile << "#bin\t";
        
        // Write channel headers
        for (Int_t ch = 0; ch < CHANNELS; ch++)
        {
            outputFile << "ch" << ch << "\t";
        }
        outputFile << "\n";
        
        // Write Front detector data
        for(Int_t bin = 0; bin < SAMPLINGS; bin++)
        {
            outputFile << bin << "\t";
            for(Int_t ch = 0; ch < CHANNELS; ch++)
            {
                outputFile << fFront[ev][ch][bin] << "\t";
            }
            outputFile << endl;
        }

        // Write a separator and header for the back detector
        outputFile << "#" << endl;
        outputFile << "#Back Detector" << endl;
        outputFile << "#bin\t";

        // Write channel headers for the Back detector
        for(Int_t ch = 0; ch < CHANNELS; ch++)
        {
            outputFile << "ch" << ch << "\t";
        }
        outputFile << "\n";

        // Write Back detector data
        for(Int_t bin = 0; bin < SAMPLINGS; bin++)
        {
            outputFile << bin << "\t";
            for(Int_t ch = 0; ch < CHANNELS; ch++)
            {
                outputFile << fBack[ev][ch][bin] << "\t";
            }
            outputFile << endl;
        }
    }

    // Close the output file
    outputFile.close();

    // Display a message indicating where the data was saved
    cout << "\nData saved in " << outputFilename << endl;
}
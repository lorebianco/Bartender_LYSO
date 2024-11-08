/** 
 * @file bar.cc
 * @brief Definition of the class BarLYSO
 */
#include "bar.hh"

using namespace std;
using namespace TMath;


BarLYSO::BarLYSO(const char*inputFilename, Int_t threadID)
{
    // Set the threadID
    fThreadID = threadID;

    // Initialize the random generators
    fRandPars = new TRandom3(0);
    fRandNoise = new TRandom3(0);

    // Initialize DAQ
    fDAQ = new DAQ();

    // Initialize whole WF containers [CHANNELS]x[SAMPLINGS]
    fEvent = -1;
    fFront.resize(CHANNELS, vector<Double_t>(SAMPLINGS, 0));
    fBack.resize(CHANNELS, vector<Double_t>(SAMPLINGS, 0)); 
    fTimes_F.resize(CHANNELS, vector<Double_t>(SAMPLINGS, 0)); 
    fTimes_B.resize(CHANNELS, vector<Double_t>(SAMPLINGS, 0)); 

    // Determine the output filename based on the BarLYSO ID
    string outputFilename = GenerateOutputFilename(inputFilename);

    // Create the file.root and the TTree
    fOutFile = TFile::Open(outputFilename.c_str(), "RECREATE");        
    fOutTree = new TTree("lyso_wfs", "lyso_wfs");
    fOutTree->Branch("Event", &fEvent);
    fOutTree->Branch("Time_F", &fTimes_F);
    fOutTree->Branch("Time_B", &fTimes_B);
    fOutTree->Branch("Front", &fFront);
    fOutTree->Branch("Back", &fBack);
}



BarLYSO::~BarLYSO()
{
    // Delete DAQ and random generators
    delete fDAQ;
    delete hPars;
    delete fRandPars;
    delete fRandNoise;

    // Ensure that we delete the TTree and TFile objects only if they are not null
    if(fOutTree)
    {
        fOutTree->ResetBranchAddresses();  // Reset any attached branches if needed
        delete fOutTree;
    }
    if(fOutFile)
    {
        fOutFile->Close(); // Make sure to close the file properly
        delete fOutFile;
    }
}



string BarLYSO::GenerateOutputFilename(const char* inputFilename)
{
    // Regular expression to extract digits following "MCID_"
    regex regex("\\bMCID_(\\d+)");

    // Storage for matched results
    smatch match;

    // Convert inputFilename to string
    string filename = inputFilename;

    // Check if there is a match for digits following "MCID_" in the filename
    if(regex_search(filename, match, regex))
    {
        // Extract the matched digits as a string
        string runID = match[1].str();

        if (!runID.empty())
        {
            // Convert the extracted string to an integer and assign it to the fID member variable
            fID = stoi(runID);
        }
    }

    // Determine the output filename based on the Bar ID
    string outputFilename;
    if (fID)
        outputFilename = "BarID_" + to_string(fID) + "_t" + to_string(fThreadID) + ".root";
    else
        outputFilename = "output.root";

    return outputFilename;
}



void BarLYSO::SetSamplingTimes()
{
    // Set the times
    for(Int_t j = 0; j < CHANNELS; j++)
    {
        if(fDAQ->fIsBinSizeConstant)
        {
            for(Int_t i = 0; i < SAMPLINGS; i++)
            {
                fTimes_F[j][i] = (Double_t) i / fDAQ->fSamplingSpeed;
                fTimes_B[j][i] = (Double_t) i / fDAQ->fSamplingSpeed;
            }
        }
        else
        {
            // Initialize the first time points
            fTimes_F[j][0] = 0.0;
            fTimes_B[j][0] = 0.0;

            for(Int_t i = 0; i < SAMPLINGS - 1; i++)  // Up to SAMPLINGS - 1
            {
                Double_t bin_F, bin_B;
                do
                {
                    bin_F = fDAQ->binRand->Gaus(1.0 / fDAQ->fSamplingSpeed, fDAQ->fSigmaBinSize);
                } while (bin_F < 0.5 * (1.0 / fDAQ->fSamplingSpeed) || bin_F > 1.5 * (1.0 / fDAQ->fSamplingSpeed));

                do
                {
                    bin_B = fDAQ->binRand->Gaus(1.0 / fDAQ->fSamplingSpeed, fDAQ->fSigmaBinSize);
                } while (bin_B < 0.5 * (1.0 / fDAQ->fSamplingSpeed) || bin_B > 1.5 * (1.0 / fDAQ->fSamplingSpeed));

                fTimes_F[j][i+1] = fTimes_F[j][i] + bin_F;
                fTimes_B[j][i+1] = fTimes_B[j][i] + bin_B;
            }
        }
    }
}



void BarLYSO::SetParsDistro()
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
 
    // Delete the TTree
    delete tree;
}



Double_t BarLYSO::Wave_OnePhel(Double_t t, Double_t A, Double_t tau_rise, Double_t tau_dec, Double_t timePhel)
{
    Double_t funcVal;
    Double_t expRise = Exp(-(t-timePhel)/tau_rise);
    Double_t expDec = Exp(-(t-timePhel)/tau_dec);

    // It happens sometimes when t << timePhel or t >> timePhel, so it's just a numerical fixing
    if(expRise > DBL_MAX || expRise < DBL_MIN) expRise = 0;
    if(expDec > DBL_MAX || expDec < DBL_MIN) expDec = 0;

    // 1-Phel function
    funcVal = A*(expRise-expDec)*((t > timePhel) ? 1:0);    
    return funcVal;
}



void BarLYSO::InitializeBaselines(Int_t event)
{
    fEvent = event;
    fFront.assign(CHANNELS, std::vector<Double_t>(SAMPLINGS, 0.));
    fBack.assign(CHANNELS, std::vector<Double_t>(SAMPLINGS, 0.));
}



void BarLYSO::ClearContainers()
{
    for(Int_t ch = 0; ch < CHANNELS; ch++)
    {
        fFront[ch].clear();
        fBack[ch].clear();
    }

    fFront.clear();
    fBack.clear();
}



void BarLYSO::SetFrontWaveform(Int_t channel, Double_t start)
{
    // Sample from hPars the parameters of 1-Phel WF
    Double_t A, tau_rise, tau_dec;
    hPars->GetRandom3(A, tau_rise, tau_dec, fRandPars);
    
    // Evaluate and sum the new 1-Phel WF to the existing one
    for(Int_t bin = 0; bin < SAMPLINGS; bin++)
    {
        fFront[channel][bin] += Wave_OnePhel(fTimes_F[channel][bin], A, tau_rise, tau_dec, start + ZERO_TIME_BIN);
    }
}



void BarLYSO::SetBackWaveform(Int_t channel, Double_t start)
{
    // Sample from hPars the parameters of 1-Phel WF
    Double_t A, tau_rise, tau_dec;
    hPars->GetRandom3(A, tau_rise, tau_dec, fRandPars);
    
    // Evaluate and sum the new 1-Phel WF to the existing one
    for(Int_t bin = 0; bin < SAMPLINGS; bin++)
    {
        fBack[channel][bin] += Wave_OnePhel(fTimes_B[channel][bin], A, tau_rise, tau_dec, start + ZERO_TIME_BIN);
    }
}



void BarLYSO::SaveEvent()
{   
    // Recompute the gain and add noise
    Double_t k = fDAQ->ComputeFactorOfGainConversion();
    for(Int_t ch = 0; ch < CHANNELS; ch++)
    {
        for(Int_t bin = 0; bin < SAMPLINGS; bin++)
        {
            // Set every bin with gaussian noise
            fFront[ch][bin] = k*fFront[ch][bin] + Add_Noise();
            fBack[ch][bin] = k*fBack[ch][bin] + Add_Noise();
        }
    }

    fOutTree->Fill();
}



void BarLYSO::SaveBar()
{
    fOutFile->cd();
    fOutTree->Write("lyso_wfs");
}
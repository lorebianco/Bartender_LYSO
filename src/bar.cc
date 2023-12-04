/** 
 * @file bar.cc
 * @brief Definition of class Bar
 */
#include "bar.hh"


Bar::Bar(const char*inputFilename)
{
    regex regex("\\d+");
    smatch match;
    string filename = inputFilename;
    if(regex_search(filename, match, regex))
    {
        string runID = match.str();
        if(!runID.empty())
        {
            fID = stoi(runID);
        }
    }
}



Bar::~Bar()
{
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
}




void Bar::SetParsDistro()
{
    Int_t status;
    Double_t my_charge, A, tau_rise, tau_dec;

    TTree *tree = new TTree("tree", "mytree");
    tree->ReadFile(fInputFilename.c_str());

    tree->SetBranchAddress("Status", &status);
    tree->SetBranchAddress("my_charge", &my_charge);
    tree->SetBranchAddress("A", &A);
    tree->SetBranchAddress("Tau_rise", &tau_rise);
    tree->SetBranchAddress("Tau_dec", &tau_dec);

    hPars = new TH3D("hPars", "Fitted All", fHisto_A[0], fHisto_A[1], fHisto_A[2], fHisto_Tau_rise[0], fHisto_Tau_rise[1], fHisto_Tau_rise[2], fHisto_Tau_dec[0], fHisto_Tau_dec[1], fHisto_Tau_dec[2]);

    for(Int_t i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);
        if(status==0 && my_charge >= fChargeCuts[0] && my_charge <= fChargeCuts[1])       
        {
            hPars->Fill(A, tau_rise, tau_dec);
        }
    }

    cout << "Distro setted with entries with charge in [" << fChargeCuts[0] << ", " << fChargeCuts[1] << "]" << endl;

    delete tree;
}




Double_t Bar::Add_Noise() { return fRandNoise->Gaus(0, fSigmaNoise); }




Double_t Bar::Wave_OnePhel(Double_t t, Double_t A, Double_t tau_rise, Double_t tau_dec, Int_t timePhel)
{
    Double_t funcVal;
    Double_t expRise = Exp(-(t-timePhel)/tau_rise);
    Double_t expDec = Exp(-(t-timePhel)/tau_dec);

    //It happens sometimes when t << timePhel or t >> timePhel, so it's just a numerical fixing
    if(expRise > DBL_MAX || expRise < DBL_MIN) expRise = 0;
    if(expDec > DBL_MAX || expDec < DBL_MIN) expDec = 0;

    funcVal = (-A*(expDec-expRise))*((t > timePhel) ? 1:0);

    return funcVal;
}




void Bar::InitializeBaselines(Int_t newEvents)
{
    EVENTS = newEvents;

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
                fFront[ev][ch][bin] = Add_Noise();
                fBack[ev][ch][bin] = Add_Noise();
            }
        }
    }

    return;
}




void Bar::SetFrontWaveform(Int_t event, Int_t channel, Double_t start)
{
    Double_t A, tau_rise, tau_dec;
    hPars->GetRandom3(A, tau_rise, tau_dec, fRandPars);
    
    for(Int_t bin = 0; bin < SAMPLINGS; bin++)
    {
        fFront[event][channel][bin] += Wave_OnePhel(bin, A, tau_rise, tau_dec, Nint(start+ZERO_TIME_BIN));
    }
    return;
}




void Bar::SetBackWaveform(Int_t event, Int_t channel, Double_t start)
{
    Double_t A, tau_rise, tau_dec;
    hPars->GetRandom3(A, tau_rise, tau_dec, fRandPars);
    
    for(Int_t bin = 0; bin < SAMPLINGS; bin++)
    {
        fBack[event][channel][bin] += Wave_OnePhel(bin, A, tau_rise, tau_dec, Nint(start+ZERO_TIME_BIN));
    }
    return;
}



void Bar::DrawFront(Int_t event, Int_t channel)
{
    //Add conditions about event
    if(channel >= 0 && channel < CHANNELS)
    {
        Double_t bins[SAMPLINGS];
        for(Int_t i = 0; i < SAMPLINGS; i++)
        {
            bins[i] = i;
        }

        TGraph *graph = new TGraph(SAMPLINGS, bins, fFront[event][channel]);
        TCanvas *c1 = new TCanvas();
        graph->Draw();
        c1->SaveAs("wave.png");
    }
    else
    {
        cerr << "Invalid setting! Channel must be in [0,114]" << endl;
    }
    return;
}




void Bar::DrawBack(Int_t event, Int_t channel)
{
    //Add conditions about event
    if(channel >= 0 && channel < CHANNELS)
    {
        Double_t bins[SAMPLINGS];
        for(Int_t i = 0; i < SAMPLINGS; i++)
        {
            bins[i] = i;
        }

        TGraph *graph = new TGraph(SAMPLINGS, bins, fBack[event][channel]);
        TCanvas *c1 = new TCanvas();
        graph->Draw();
        c1->SaveAs("wave.png");
    }
    else
    {
        cerr << "Invalid setting! Channel must be in [0,114]" << endl;
    }
    return;
}




void Bar::SaveBar()
{   
    string outputFilename;
    if(fID)
    {
        outputFilename = "BarID_" + to_string(fID) + ".txt";
    }
    else outputFilename = "output.txt";


    ofstream outputFile(outputFilename, ofstream::out | ofstream::trunc);

    if(!outputFile.is_open())
    {
        cout << "Can't open output file!" << endl;
        return;
    }

    for(Int_t ev = 0; ev < EVENTS; ev++)
    {
        outputFile << "#Event " << ev << endl;
        outputFile << "#" << endl;
        outputFile << "#Front Detector" << endl;
        outputFile << "#bin\t";
        for (Int_t ch = 0; ch < CHANNELS; ch++)
        {
            outputFile << "ch" << ch << "\t";
        }
        outputFile << "\n";

        for(Int_t bin = 0; bin < SAMPLINGS; bin++)
        {
            outputFile << bin << "\t";
            for(Int_t ch = 0; ch < CHANNELS; ch++)
            {
                outputFile << fFront[ev][ch][bin] << "\t";
            }
            outputFile << endl;
        }
        outputFile << "#" << endl;
        outputFile << "#Back Detector" << endl;
        outputFile << "#bin\t";
        for(Int_t ch = 0; ch < CHANNELS; ch++)
        {
            outputFile << "ch" << ch << "\t";
        }
        outputFile << "\n";

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

    outputFile.close();

    cout << "\nData saved in " << outputFilename << endl;
}

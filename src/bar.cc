/** 
 * @file bar.cc
 * @brief Definition of class Bar
 */
#include "bar.hh"


Bar::Bar(Int_t events, const char*inputFilename) : EVENTS(events)
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

    
    fFront = new Double_t**[EVENTS];
    fBack = new Double_t**[EVENTS];

    for(Int_t i = 0; i < EVENTS; i++)
    {
        fFront[i] = new Double_t*[CHANNELS];
        fBack[i] = new Double_t*[CHANNELS];
        
        for(Int_t j = 0; j < CHANNELS; j++)
        {
            fFront[i][j] = new Double_t[SAMPLINGS];
            fBack[i][j] = new Double_t[SAMPLINGS];
        }
    }

    InitializeBaselines();

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




Double_t Bar::Add_Noise() { return fRandNoise->Gaus(0, sigmaNoise); }




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




void Bar::InitializeBaselines()
{
    for(Int_t ev = 0; ev < EVENTS; ev++)
    {
        for(Int_t ch = 0; ch < CHANNELS; ch++)
        {
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
    hAll->GetRandom3(A, tau_rise, tau_dec, fRandPars);
    
    for(Int_t bin = 0; bin < SAMPLINGS; bin++)
    {
        fFront[event][channel][bin] += Wave_OnePhel(bin, A, tau_rise, tau_dec, Nint(start+ZERO_TIME_BIN));
    }
    return;
}




void Bar::SetBackWaveform(Int_t event, Int_t channel, Double_t start)
{
    Double_t A, tau_rise, tau_dec;
    hAll->GetRandom3(A, tau_rise, tau_dec, fRandPars);
    
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

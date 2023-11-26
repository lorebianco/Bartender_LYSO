#include "run.hh"

Run::Run(Int_t events) : EVENTS(events)
{
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

    InitializeFrontWaveforms();
    InitializeBackWaveforms();

}



Run::~Run()
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




void Run::InitializeFrontWaveforms()
{
    for(Int_t ev = 0; ev < EVENTS; ev++)
    {
        for(Int_t ch = 0; ch < CHANNELS; ch++)
        {
            for(Int_t bin = 0; bin < SAMPLINGS; bin++)
            {
                fFront[ev][ch][bin] = Add_Noise(SIGMA_NOISE);
            }
        }
    }

    return;
}




void Run::InitializeBackWaveforms()
{
    for(Int_t ev = 0; ev < EVENTS; ev++)
    {
        for(Int_t ch = 0; ch < CHANNELS; ch++)
        {
            for(Int_t bin = 0; bin < SAMPLINGS; bin++)
            {
                fBack[ev][ch][bin] = Add_Noise(SIGMA_NOISE);
            }
        }
    }

    return;
}




void Run::SetFrontWaveform(Int_t event, Int_t channel, Double_t A, Double_t tau_rise, Double_t tau_dec, Double_t start)
{
    for(Int_t bin = 0; bin < SAMPLINGS; bin++)
    {
        fFront[event][channel][bin] += Wave_OnePhel(bin, A, tau_rise, tau_dec, Nint(start+ZERO_TIME_BIT));
    }
    return;
}




void Run::SetBackWaveform(Int_t event, Int_t channel, Double_t A, Double_t tau_rise, Double_t tau_dec, Double_t start)
{
   for(Int_t bin = 0; bin < SAMPLINGS; bin++)
    {
        fBack[event][channel][bin] += Wave_OnePhel(bin, A, tau_rise, tau_dec, Nint(start+ZERO_TIME_BIT));
    }
    return;
}



void Run::DrawFront(Int_t event, Int_t channel)
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



/*void Run::DrawBackChannel(Int_t channel)
{
    if(channel >= 0 && channel < CHANNELS)
    {
        new TCanvas();
        fBack[channel].DrawSampling();
    }
    else
    {
        cerr << "Invalid setting! Channel must be in [0,114]" << endl;
    }
    return;
}*/



/*void Run::SaveEventData(Int_t eventNum, const char* outputFilename)
{
    ofstream outputFile(outputFilename, ios::out | ios::app);
    if (outputFile.is_open())
    {
        outputFile << "#Event number " << eventNum << "\n#\n";

        // Writing Front Detector data for the event
        outputFile << "#Front Detector\n#bin\t";
        for (Int_t ch = 0; ch < CHANNELS; ++ch)
        {
            outputFile << "ch" << ch << " [V]\t";
        }
        outputFile << "\n";

        for (Int_t i = 0; i < SAMPLINGS; ++i)
        {
            outputFile << fFront[0].fBins[i] << "\t";
            for (Int_t ch = 0; ch < CHANNELS; ++ch)
            {
                outputFile << fFront[ch].fSamples[i] << "\t";
            }
            outputFile << "\n";
        }
        outputFile << "\n";

        // Writing Back Detector data for the event
        outputFile << "#Back Detector\n#bin\t";
        for (Int_t ch = 0; ch < CHANNELS; ++ch)
        {
            outputFile << "ch" << ch << " [V]\t";
        }
        outputFile << "\n";

        for (Int_t i = 0; i < SAMPLINGS; ++i)
        {
            outputFile << fBack[0].fBins[i] << "\t";
            for (Int_t ch = 0; ch < CHANNELS; ++ch)
            {
                outputFile << fBack[ch].fSamples[i] << "\t";
            }
            outputFile << "\n";
        }
        outputFile << "\n";
        
        outputFile.close();
        if(eventNum == 0) cout << "Event data will be saved in " << outputFilename << endl;
    }
    else
    {
        cerr << "Can't open the output file!" << endl;
    }

    return;
}*/
#include "event.hh"

Event::Event()
{}



Event::~Event()
{}



void Event::SetFrontChannel(Int_t channel, TF1 *FinalWave)
{
    if(channel >= 0 && channel < CHANNELS)
    {
        fFront[channel].SetSamples(FinalWave);
    }
    else
    {
        cerr << "Invalid setting! Channel must be in [0,114]" << endl;
    }
    return;
}



void Event::SetBackChannel(Int_t channel, TF1 *FinalWave)
{
    if(channel >= 0 && channel < CHANNELS)
    {
        fBack[channel].SetSamples(FinalWave);
    }
    else
    {
        cerr << "Invalid setting! Channel must be in [0,114]" << endl;
    }
    return;
}



void Event::DrawFrontChannel(Int_t channel)
{
    if(channel >= 0 && channel < CHANNELS)
    {
        new TCanvas();
        fFront[channel].DrawSampling();
    }
    else
    {
        cerr << "Invalid setting! Channel must be in [0,114]" << endl;
    }
    return;
}



void Event::DrawBackChannel(Int_t channel)
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
}



void Event::SaveEventData(Int_t eventNum, const char* outputFilename)
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
        cout << "Data saved in " << outputFilename << endl;
    }
    else
    {
        cerr << "Can't open the output file!" << endl;
    }

    return;
}
#include <iostream>
#include <fstream>

using namespace std;

// Number of channels
constexpr Int_t CHANNELS = 115;
// Number of samples
constexpr Int_t SAMPLINGS = 1024;

// Reads samples from a given file for a specific event, detector type, and channel
Bool_t ReadSamples(const char* filename, Int_t eventNumber, Bool_t isFront, Int_t channelNumber, Double_t eventData[])
{
    ifstream inputFile(filename);

    // Check if the input file can be opened
    if(!inputFile.is_open())
    {
        cerr << "Can't open input file!" << endl;
        return false;
    }

    string line;
    // Determine the event and detector headers
    string detectorType = isFront ? "#Front Detector" : "#Back Detector";
    string eventHeader = "#Event " + to_string(eventNumber);

    Bool_t foundEvent = false;
    Bool_t foundChannel = false;

    Bool_t readData = false;
    Int_t currentChannel = -1;

    // Find the waveform in the input file
    while(getline(inputFile, line))
    {
        // Look for the specific event
        if(line == eventHeader)
        {
            foundEvent = true;
            getline(inputFile, line); // Skip the blank line after the event header

            while(getline(inputFile, line))
            {
                // Look for the specific detector
                if(line == detectorType)
                {
                    readData = true;
                    break;
                }
            }

            if(readData)
            {
                while(getline(inputFile, line) && line != "#")
                {
                    Int_t bin;
                    inputFile >> bin;

                    // Look for the specific channel
                    for(Int_t ch = 0; ch < CHANNELS; ch++)
                    {
                        Double_t data;
                        inputFile >> data;

                        // Store samplings
                        if(ch == channelNumber)
                        {
                            eventData[bin] = data;
                            foundChannel = true;
                        }
                    }
                }
            }

            break;
        }
    }

    inputFile.close();

    // Check if the event or channel was found
    if (!foundEvent || !foundChannel)
    {
        cerr << "Can't find event or channel!" << endl;
        return false;
    }

    return true;
}


// Draws waveforms from a specified event, detector and channel
void DrawSamples(const char* filename, int eventNumberToLoad, bool isFront, int channelNumberToLoad)
{
    Double_t eventData[SAMPLINGS];

    // Load data related to the specified event, detector and channel into the eventData array
    Bool_t success = ReadSamples(filename, eventNumberToLoad, isFront, channelNumberToLoad, eventData);

    if(success)
    {
        Double_t bins[SAMPLINGS];
        for(Int_t i = 0; i < SAMPLINGS; i++)
        {
            bins[i] = i;
        }
        TGraph *graph = new TGraph(SAMPLINGS, bins, eventData);

        // Set a title for the plot
        TString detector;
        if(isFront) detector = "Front-";
        else detector = "Back-"; 
        TString title = "Event " + TString::Format("%d", eventNumberToLoad) + "    " + detector + "ch" + TString::Format("%d", channelNumberToLoad);        
        graph->SetTitle(title.Data());
        TCanvas *c1 = new TCanvas();
        graph->Draw("APL");
    }
}

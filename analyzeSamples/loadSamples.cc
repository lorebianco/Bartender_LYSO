#include <iostream>
#include <fstream>

using namespace std;

#define CHANNELS 115
#define SAMPLINGS 1024

Bool_t ReadSamples(const char* filename, Int_t eventNumber, Bool_t isFront, Int_t channelNumber, Double_t eventData[])
{
    ifstream inputFile(filename);

    if(!inputFile.is_open()) {
        cout << "Can't open input file!" << endl;
        return false;
    }

    string line;
    string eventType = isFront ? "#Front Detector" : "#Back Detector";
    string eventHeader = "#Event " + to_string(eventNumber);

    Bool_t foundEvent = false;
    Bool_t foundChannel = false;

    Bool_t readData = false;
    Int_t currentChannel = -1;

    // Trova l'evento appropriato (Front o Back) nel file di input
    while(getline(inputFile, line))
    {
        if(line == eventHeader)
        {
            foundEvent = true;
            getline(inputFile, line); // Salta la riga vuota dopo l'intestazione dell'evento

            // Cerca il tipo di evento specifico (Front o Back)
            while(getline(inputFile, line))
            {
                if(line == eventType)
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

                    for(Int_t ch = 0; ch < CHANNELS; ch++)
                    {
                        Double_t data;
                        inputFile >> data;

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

    if (!foundEvent || !foundChannel)
    {
        cout << "Can't find event or channel!" << endl;
        return false;
    }

    return true;
}




void DrawSamples(const char* filename, int eventNumberToLoad, bool isFront, int channelNumberToLoad)
{
    Double_t eventData[SAMPLINGS];

    // Carica i dati Front relativi all'evento specificato e al canale specificato nell'array eventData
    Bool_t success = ReadSamples(filename, eventNumberToLoad, isFront, channelNumberToLoad, eventData);

    if(success)
    {
        Double_t bins[SAMPLINGS];
        for(Int_t i = 0; i < SAMPLINGS; i++)
        {
            bins[i] = i;
        }
        TGraph *graph = new TGraph(SAMPLINGS, bins, eventData);

        TString detector;
        if(isFront) detector = "Front-";
        else detector = "Back-"; 
        TString title = "Event " + TString::Format("%d", eventNumberToLoad) + "    " + detector + "ch" + TString::Format("%d", channelNumberToLoad);        
        graph->SetTitle(title.Data());
        TCanvas *c1 = new TCanvas();
        graph->Draw("APL");
    }
}

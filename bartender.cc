////////////////////////////////////////////////////////////////////////////////
//                        Lorenzo Bianco 13/11/2023                           //
//                                                                            //
//             My 'Bartender' for the LYSO calorimeter prototype              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <vector>
#include <cfloat>

using namespace std;
using namespace TMath;

#define CHANNELS 116

TRandom3 *randAll = new TRandom3(0);
TRandom3 *randNoise = new TRandom3(0);


TH1D *hA, *hTauRise, *hTauDec;
TH2D *hATd, *hATr, *hTdTr;
TH3D *hAll;


void SetDistros(const char *dataFilename, Double_t cutChargeMin = DBL_MIN, Double_t cutChargeMax = DBL_MAX)
{
    Int_t status;
    Double_t my_charge, A, tau_rise, tau_dec;

    TTree *tree = new TTree("tree","mytree");
    tree->ReadFile(dataFilename);

    tree->SetBranchAddress("Status", &status);
    tree->SetBranchAddress("my_charge", &my_charge);
    tree->SetBranchAddress("A", &A);
    tree->SetBranchAddress("Tau_rise", &tau_rise);
    tree->SetBranchAddress("Tau_dec", &tau_dec);

    hA = new TH1D("hA", "Fitted A distro", 50 ,0, 0.6);
    hTauRise = new TH1D("hTauRise", "Fitted Tau_Rise distro", 50, 0, 1.5);
    hTauDec = new TH1D("hTauDec", "Fitted Tau_Dec", 50, 0, 11);

    hATd = new TH2D("hATd", "Fitted A vs Tau_dec", 50 ,0, 0.6, 50, 0, 11);
    hATr = new TH2D("hATr", "Fitted A vs Tau_rise", 50 ,0, 0.6, 50, 0, 1.5);
    hTdTr = new TH2D("hTdTr", "Fitted Tau_dec vs Tau_rise", 50 ,0, 11, 50, 0, 1.5);

    hAll = new TH3D("hAll", "Fitted All", 50 ,0, 0.6, 50, 0, 1.5, 50, 0, 11);

    for(Int_t i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);
        if(status==0 && my_charge >= cutChargeMin && my_charge <= cutChargeMax)       
        {
            hA->Fill(A);
            hTauRise->Fill(tau_rise);
            hTauDec->Fill(tau_dec);

            hATd->Fill(A, tau_dec);
            hATr->Fill(A, tau_rise);
            hTdTr->Fill(tau_dec, tau_rise);

            hAll->Fill(A, tau_rise, tau_dec);
        }
    }

    new TCanvas("c1");
    hA->Draw("HIST");
    
    new TCanvas("c2");
    hTauRise->Draw("HIST");
    
    new TCanvas("c3");
    hTauDec->Draw("HIST");

    new TCanvas("c4");
    hATd->Draw("colz");

    new TCanvas("c5");
    hATr->Draw("colz");

    new TCanvas("c6");
    hTdTr->Draw("colz");

}



class Sample
{
public:
    Sample()
    {
        for(Int_t i = 0; i < 1024; i++)
        {
            fBins[i] = i;
        }
    }
    ~Sample() {; }

    void SetSamples(TF1 *FinalWave)
    {
        for(Int_t i = 0; i < 1024; i++)
        {
            fSamples[i] = FinalWave->Eval(fBins[i]);
        }
    }

    void DrawSampling()
    {
        TGraph *gWaveform = new TGraph(1024, fBins, fSamples);
        //new TCanvas("cGraph");
        
        //gWaveform->SetMarkerStyle(20);
        gWaveform->Draw();
    }

    void SaveSampling(const char* outputFilename)
    {
        ofstream outputFile(outputFilename, ios::out | ios::trunc);
        if (outputFile.is_open())
        {
            //Header of the file
            outputFile << "#bin\tvoltage [V]\n";

            //Writing data
            for (Int_t i = 0; i < 1024; i++)
            {
                outputFile << fBins[i] << "\t" << fSamples[i] << "\n";
            }

            outputFile.close();

            cout << "Data saved in sampling_data.txt" << endl;
        }
        else
        {
            cerr << "Can't open the output file!" << endl;
        }
    }

    Double_t fBins[1024];
    Double_t fSamples[1024];
};




class Event
{
public:
    Event() { ;}
    ~Event() { ;}

    Sample fFront[CHANNELS];
    Sample fBack[CHANNELS];

    void SetChannel(Int_t ForB, Int_t channel, TF1 *FinalWave)
    {
        if(ForB == 0 && channel >= 0 && channel < CHANNELS)
        {
            fFront[channel].SetSamples(FinalWave);
        }
        else if(ForB == 1 && channel >= 0 && channel < CHANNELS)
        {
            fBack[channel].SetSamples(FinalWave);
        }
        else
        {
            cerr << "Invalid settings! Valid settings are 0 for Front or 1 for Back and 0 <= channel < 116 !" << endl;
        }
        return;
    }

    void DrawChannel(Int_t ForB, Int_t channel)
    {
        if(ForB == 0 && channel >= 0 && channel < CHANNELS)
        {
            new TCanvas();
            fFront[channel].DrawSampling();
        }
        else if(ForB == 1 && channel >= 0 && channel < CHANNELS)
        {
            new TCanvas();
            fBack[channel].DrawSampling();
        }
        else
        {
            cerr << "Invalid settings! Valid settings are 0 for Front or 1 for Back and 0 <= channel < 116 !" << endl;
        }
        return;
    }
};




class Run
{
public:
    Run(Int_t nEvents)
    { 
        fEvents = new Event[nEvents];
        fNumberofEvents = nEvents;
    }
    ~Run() { delete[] fEvents;}

    Event *fEvents;

    void SaveRunData(const char* outputFilename)
    {
        ofstream outputFile(outputFilename, ios::out | ios::trunc);
        if (outputFile.is_open())
        {
            for (Int_t eventNum = 0; eventNum < fNumberofEvents; eventNum++)
            {
                outputFile << "#Event number " << eventNum << "\n#\n";

                // Writing Front Detector data for the event
                outputFile << "#Front Detector\n#bin\t";
                for (Int_t ch = 0; ch < CHANNELS; ++ch)
                {
                    outputFile << "ch" << ch << " [V]\t";
                }
                outputFile << "\n";

                for (Int_t i = 0; i < 1024; ++i)
                {
                    outputFile << fEvents[eventNum].fFront[0].fBins[i] << "\t";
                    for (Int_t ch = 0; ch < CHANNELS; ++ch)
                    {
                        outputFile << fEvents[eventNum].fFront[ch].fSamples[i] << "\t";
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

                for (Int_t i = 0; i < 1024; ++i)
                {
                    outputFile << fEvents[eventNum].fBack[0].fBins[i] << "\t";
                    for (Int_t ch = 0; ch < CHANNELS; ++ch)
                    {
                        outputFile << fEvents[eventNum].fBack[ch].fSamples[i] << "\t";
                    }
                    outputFile << "\n";
                }
                outputFile << "\n";
            }

            outputFile.close();
            cout << "Data saved in " << outputFilename << endl;
        }
        else
        {
            cerr << "Can't open the output file!" << endl;
        }
    }

private:
    Int_t fNumberofEvents;
};




Double_t Add_Noise(Double_t *x, Double_t *par) {    return randNoise->Gaus(0, par[0]); }




struct SumWaves
{ 
    SumWaves(const vector<TF1 *> & flist) : fFuncList(flist) {}

    Double_t operator() (const Double_t *x, const Double_t *p)
    {
        Double_t result = 0;
        for (Int_t i = 0; i < fFuncList.size(); i++) 
            result += fFuncList[i]->EvalPar(x,p);
        return result; 
    }

    vector<TF1*> fFuncList;
      
};




Double_t Wave_OnePhel(Double_t *x, Double_t *par)
{
    Double_t funcVal;
    Double_t expRise = Exp(-(x[0]-par[3])/par[1]);
    Double_t expDec = Exp(-(x[0]-par[3])/par[2]);

    //It happens sometimes when x << start or x >> start, so it's just a numerical fixing
    if(expRise > DBL_MAX || expRise < DBL_MIN) expRise = 0;
    if(expDec > DBL_MAX || expDec < DBL_MIN) expDec = 0;

    funcVal = (-par[0]*(expDec-expRise))*((x[0]>par[3]) ? 1:0);

    return funcVal;
}




TF1* Bartender(Int_t N_phel, Int_t *times)
{
    vector<TF1 *> bar;

    //First of all set a baseline with noise
    TF1 *noise = new TF1("Noise", Add_Noise, 0, 1023, 1);
    noise->SetParameter(0, 0.02);
    bar.push_back(noise);

    for(Int_t i = 0; i < N_phel; i++)
    {
        Double_t A, tau_rise, tau_dec;

        hAll->GetRandom3(A, tau_rise, tau_dec, randAll);
        TF1 *wave = new TF1("wave", Wave_OnePhel, 0, 1023, 4);
        wave->SetParameters(A, tau_rise, tau_dec, times[i]);
        
        bar.push_back(wave);
    } 
    
    TF1 *FinalWave = new TF1("FinalWave", SumWaves(bar), 0, 1023, 0);

    return FinalWave;
}




Int_t Start(const char *inputFilename)
{
    if(hAll==nullptr)
    {
        SetDistros("/home/lorenzo/MEG_Project/Acquisizioni/Studio_SIPM/Preamp_Ext/Dati19_10/Dati_root/dati_spectrum_T20_V5478_1pe_fit_params.txt", 0, 1.5);
    }

    TFile *mcFile = TFile::Open(inputFilename, "READ");
    TTree *tFront = mcFile->Get<TTree>("F");
    TTree *tBack = mcFile->Get<TTree>("B");
    TTree *tCry = mcFile->Get<TTree>("Cry");

    tFront->SetBranchStatus("*", false);
    tBack->SetBranchStatus("*", false);
    tCry->SetBranchStatus("*", false);

    tFront->SetBranchStatus("fEvent", true);
    tFront->SetBranchStatus("fT", true);
    tFront->SetBranchStatus("fChannel", true);
    tBack->SetBranchStatus("fEvent", true);
    tBack->SetBranchStatus("fT", true);
    tBack->SetBranchStatus("fChannel", true);
    tCry->SetBranchStatus("fEvent", true);
    tCry->SetBranchStatus("fTin", true);


    Int_t fEvent_front, fEvent_back, fChannel_front, fChannel_back;
    Double_t fT_front, fT_back;

    tFront->SetBranchAddress("fEvent", &fEvent_front);
    tBack->SetBranchAddress("fEvent", &fEvent_back);

    tFront->SetBranchAddress("fChannel", &fChannel_front);
    tBack->SetBranchAddress("fChannel", &fChannel_back);

    tFront->SetBranchAddress("fT", &fT_front);
    tBack->SetBranchAddress("fT", &fT_back);

    if(!(tFront->GetMaximum("fEvent") == tBack->GetMaximum("fEvent") && tBack->GetMaximum("fEvent") == tCry->GetMaximum("fEvent"))) 
    {
        cerr << "Error: something wrong in the MC file happened: events in trees are not the same" << endl;
        return 1;
    }

    Int_t nEvents = tFront->GetMaximum("fEvent") + 1;

    Run run(nEvents);

    for(Int_t i = 0; i < nEvents; i++)
    {
        for(Int_t j = 0; j < CHANNELS; j++)
        {
            string selection = "fEvent == " + to_string(i) + " && fChannel == " + to_string(j);
            const char *charSelection = selection.c_str();

            Int_t N_phel_front = tFront->GetEntries(charSelection);
            Int_t *times_front = new Int_t[N_phel_front];

            Int_t N_phel_back = tBack->GetEntries(charSelection);
            Int_t *times_back = new Int_t[N_phel_back];

            Int_t indexTimes_front = 0, indexTimes_back = 0;

            for(Int_t k = 0; k < tFront->GetEntries(); k++)
            {
                tFront->GetEntry(k);
                if(fEvent_front == i && fChannel_front == j)
                {
                    times_front[indexTimes_front] = Nint(fT_front+450); 
                    indexTimes_front++;
                }
            }
            for(Int_t k = 0; k < tBack->GetEntries(); k++)
            {
                tBack->GetEntry(k);

                if(fEvent_back == i && fChannel_back == j)
                {
                    times_back[indexTimes_back] = Nint(fT_back+450); 
                    indexTimes_back++; 
                }
            }
            
            run.fEvents[i].SetChannel(0, j, Bartender(N_phel_front, times_front));
            if(j == 79) run.fEvents[i].DrawChannel(0, j);
            run.fEvents[i].SetChannel(1, j, Bartender(N_phel_back, times_back));

            //delete[] times_front;
            //delete[] times_back;
        }
    }

    run.SaveRunData("output.txt");

    mcFile->Close();
    return 0;
} 






/*
Int_t Single_Start(Int_t EVENT, Int_t CHANNEL)
{
    //TH1D *hCharge = new TH1D("hCharge", "Charge", 29, 0, 1.4);
    if(hAll==nullptr)
    {
        SetDistros("/home/lorenzo/MEG_Project/Acquisizioni/Studio_SIPM/Preamp_Ext/Dati19_10/Dati_root/dati_spectrum_T20_V5478_1pe_fit_params.txt", 0, 1.5);
    }

    //ROOT::EnableImplicitMT(16);
    
    TFile *mcFile = TFile::Open("/home/lorenzo/MEG_Project/Simulazioni/MC_LYSO/build/output_MCID_run0.root", "READ");
    TTree *tFront = mcFile->Get<TTree>("F");
    TTree *tBack = mcFile->Get<TTree>("B");
    TTree *tCry = mcFile->Get<TTree>("Cry");

    tFront->SetBranchStatus("*", false);
    tBack->SetBranchStatus("*", false);
    tCry->SetBranchStatus("*", false);

    tFront->SetBranchStatus("fEvent", true);
    tFront->SetBranchStatus("fT", true);
    tFront->SetBranchStatus("fChannel", true);
    tBack->SetBranchStatus("fEvent", true);
    tBack->SetBranchStatus("fT", true);
    tBack->SetBranchStatus("fChannel", true);
    tCry->SetBranchStatus("fEvent", true);
    tCry->SetBranchStatus("fTin", true);


    Int_t fEvent_front, fEvent_back, fChannel_front, fChannel_back;
    Double_t fT_front, fT_back;

    tFront->SetBranchAddress("fEvent", &fEvent_front);
    tBack->SetBranchAddress("fEvent", &fEvent_back);

    tFront->SetBranchAddress("fChannel", &fChannel_front);
    tBack->SetBranchAddress("fChannel", &fChannel_back);

    tFront->SetBranchAddress("fT", &fT_front);
    tBack->SetBranchAddress("fT", &fT_back);
    
    
    //hCharge->Fill(Bartender(3000));
    string selection = "fEvent == " + to_string(EVENT) + " && fChannel == " + to_string(CHANNEL);
    const char *charSelection = selection.c_str();

    Int_t N_phel = tFront->GetEntries(charSelection);
    Int_t *times = new Int_t[N_phel];
    
    cout << "Summed " << N_phel << " photons" << endl;

    Int_t indexTimes = 0;
    for(Int_t i = 0; i < tFront->GetEntries(); i++)
    {
        tFront->GetEntry(i);
        
        if(fEvent_front == EVENT && fChannel_front == CHANNEL)
        {
            times[indexTimes] = Nint(fT_front+450); 
            indexTimes++; 
        }
    }    

    Bartender(N_phel, times);
    //if(i%100==0) cout << "Processed event number " << i << endl;

    //new TCanvas();
    //hCharge->Draw();

    mcFile->Close();
    return 0;
}
*/
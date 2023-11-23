#ifndef FUNCTIONS_HH
#define FUNCTIONS_HH

#include <TCanvas.h>

#include "globals.hh"

TRandom3 *randAll = new TRandom3(0);
TRandom3 *randNoise = new TRandom3(0);

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

    //TH1D *hA = new TH1D("hA", "Fitted A distro", 50 ,0, 0.6);
    //TH1D *hTauRise = new TH1D("hTauRise", "Fitted Tau_Rise distro", 50, 0, 1.5);
    //TH1D *hTauDec = new TH1D("hTauDec", "Fitted Tau_Dec", 50, 0, 11);

    //TH2D *hATd = new TH2D("hATd", "Fitted A vs Tau_dec", 50 ,0, 0.6, 50, 0, 11);
    //TH2D *hATr = new TH2D("hATr", "Fitted A vs Tau_rise", 50 ,0, 0.6, 50, 0, 1.5);
    //TH2D *hTdTr = new TH2D("hTdTr", "Fitted Tau_dec vs Tau_rise", 50 ,0, 11, 50, 0, 1.5);

    hAll = new TH3D("hAll", "Fitted All", 50 ,0, 0.6, 50, 0, 1.5, 50, 0, 11);

    for(Int_t i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);
        if(status==0 && my_charge >= cutChargeMin && my_charge <= cutChargeMax)       
        {
            //hA->Fill(A);
            //hTauRise->Fill(tau_rise);
            //hTauDec->Fill(tau_dec);

            //hATd->Fill(A, tau_dec);
            //hATr->Fill(A, tau_rise);
            //hTdTr->Fill(tau_dec, tau_rise);

            hAll->Fill(A, tau_rise, tau_dec);
        }
    }

    //new TCanvas("c1");
    //hA->Draw("HIST");
    //
    //new TCanvas("c2");
    //hTauRise->Draw("HIST");
    //
    //new TCanvas("c3");
    //hTauDec->Draw("HIST");
    //
    //new TCanvas("c4");
    //hATd->Draw("colz");
    //
    //new TCanvas("c5");
    //hATr->Draw("colz");
    //
    //new TCanvas("c6");
    //hTdTr->Draw("colz");

    delete tree;
}


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




Double_t Add_Noise(Double_t *x, Double_t *par) {    return randNoise->Gaus(0, par[0]); }




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




TF1* Bartender(Int_t N_phel, vector<Int_t> times)
{
    vector<TF1 *> bar;
    TF1 *wave = new TF1("wave", Wave_OnePhel, 0, 1023, 4);
    TF1 *noise = new TF1("Noise", Add_Noise, 0, 1023, 1);
    
    Double_t A, tau_rise, tau_dec;

    //First of all set a baseline with noise
    noise->SetParameter(0, 0.02);
    bar.push_back(noise);

    //Now start the parameters sampling and the sum of single ph-el WFs
    for(Int_t i = 0; i < N_phel; i++)
    {
        hAll->GetRandom3(A, tau_rise, tau_dec, randAll);
        wave->SetParameters(A, tau_rise, tau_dec, times[i]);

        bar.push_back(wave);
    } 
    
    //Finally get the total WF
    TF1 *FinalWave = new TF1("FinalWave", SumWaves(bar), 0, 1023, 0);

    return FinalWave;
}


#endif
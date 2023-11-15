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



TRandom3 *randT = new TRandom3(0);

TRandom3 *randAll = new TRandom3(0);


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




struct SumWaves
{ 
    SumWaves(const std::vector<TF1 *> & flist) : fFuncList(flist) {}

    double operator() (const double * x, const double *p)
    {
        double result = 0;
        for (unsigned int i = 0; i < fFuncList.size(); ++i) 
            result += fFuncList[i]->EvalPar(x,p); 

        return result; 
    }

    std::vector<TF1*> fFuncList;
      
};




Double_t Wave_OnePhel(Double_t *x, Double_t *par)
{
    Double_t funcVal;
    Double_t expRise = Exp(-(x[0]-par[3])/par[1]);
    if(expRise > DBL_MAX) expRise = 0; //It happens sometimes when x < start, so it's just a numerical fixing

    funcVal = (-par[0]*(Exp(-(x[0]-par[3])/par[2])-expRise))*((x[0]>par[3]) ? 1:0);

    return funcVal;
}




Double_t Bartender(Int_t N_phel, Int_t *times)
{ 
    std::vector<TF1 *> v;

    for(Int_t i = 0; i < N_phel; i++)
    {
        Double_t A, tau_rise, tau_dec;

        hAll->GetRandom3(A, tau_rise, tau_dec, randAll);

        //cout << A << " " << tau_rise << " " << tau_dec << " " << fT << endl;

        TF1 *wave = new TF1("wave", Wave_OnePhel, 400, 800, 4);
        wave->SetParameters(A, tau_rise, tau_dec, times[i]);
        
        v.push_back(wave);

        //new TCanvas();
        //wave->Draw();
    } 
    

    TF1 *FinalWave = new TF1("FinalWave", SumWaves(v), 400, 800, 0);
    
    //cout << FinalWave->Integral(400, 600, 1E-3) << endl;
    //cout << FinalWave->Integral(400, 600, 1E-6) << endl;


    new TCanvas("C_Tot");
    FinalWave->SetNpx(400);
    FinalWave->SetMarkerStyle(20);
    FinalWave->SetMarkerColor(kRed);
    FinalWave->Draw("L");               

    //return -FinalWave->Integral(400, 600);
    return 0;
}



int main()
{
    Int_t EVENT = 0;
    Int_t CHANNEL = 80;

    //TH1D *hCharge = new TH1D("hCharge", "Charge", 29, 0, 1.4);
    SetDistros("/home/lorenzo/MEG_Project/Acquisizioni/Studio_SIPM/Preamp_Ext/Dati19_10/Dati_root/dati_spectrum_T20_V5478_1pe_fit_params.txt", 0, 1.5);

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


    Int_t fEvent_front, fEvent_back, fChannel_front, fChannel_back, fEntries_front, fEntries_back;
    Double_t fT_front, fT_back;

    tFront->SetBranchAddress("fEvent", &fEvent_front);
    tBack->SetBranchAddress("fEvent", &fEvent_back);

    tFront->SetBranchAddress("fChannel", &fChannel_front);
    tBack->SetBranchAddress("fChannel", &fChannel_back);

    tFront->SetBranchAddress("fT", &fT_front);
    tBack->SetBranchAddress("fT", &fT_back);
    
    
    //hCharge->Fill(Bartender(3000));
    Int_t N_phel = tFront->GetEntries("fEvent == 0");
    Int_t *times = new Int_t[N_phel];
    
    cout << "Summed " << N_phel << " photons" << endl;

    Int_t indexTimes = 0;
    for(Int_t i = 0; i < tFront->GetEntries(); i++)
    {
        tFront->GetEntry(i);
        
        if(fEvent_front == EVENT )
        {
            times[indexTimes] = Nint(fT_front+450); 
            indexTimes++; 
        }
    }    

    Bartender(N_phel, times);
    //if(i%100==0) cout << "Processed event number " << i << endl;

    //new TCanvas();
    //hCharge->Draw();

    return 0;
}
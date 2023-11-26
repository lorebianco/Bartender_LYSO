#include "functions.hh"


void SetDistros(const char *dataFilename, Double_t cutChargeMin, Double_t cutChargeMax)
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

    hAll = new TH3D("hAll", "Fitted All", 50 ,0, 0.6, 50, 0, 1.5, 50, 0, 11);

    for(Int_t i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);
        if(status==0 && my_charge >= cutChargeMin && my_charge <= cutChargeMax)       
        {
            hAll->Fill(A, tau_rise, tau_dec);
        }
    }

    delete tree;
}




Double_t Add_Noise(Double_t sigma) { return randNoise->Gaus(0, sigma); }




Double_t Wave_OnePhel(Double_t x, Double_t A, Double_t tau_rise, Double_t tau_dec, Int_t timePhel)
{
    Double_t funcVal;
    Double_t expRise = Exp(-(x-timePhel)/tau_rise);
    Double_t expDec = Exp(-(x-timePhel)/tau_dec);

    //It happens sometimes when x << start or x >> start, so it's just a numerical fixing
    if(expRise > DBL_MAX || expRise < DBL_MIN) expRise = 0;
    if(expDec > DBL_MAX || expDec < DBL_MIN) expDec = 0;

    funcVal = (-A*(expDec-expRise))*((x > timePhel) ? 1:0);

    return funcVal;
}
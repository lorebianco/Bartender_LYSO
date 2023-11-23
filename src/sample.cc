#include "sample.hh"

Sample::Sample()
{
    for(Int_t i = 0; i < SAMPLINGS; i++)
    {
        fBins[i] = i;
    }
}




Sample::~Sample()
{}



void Sample::SetSamples(TF1 *FinalWave)
{
    for(Int_t i = 0; i < SAMPLINGS; i++)
    {
        fSamples[i] = FinalWave->Eval(fBins[i]);
    }
    
    return;
}




void Sample::DrawSampling()
{
    TGraph *gWaveform = new TGraph(SAMPLINGS, fBins, fSamples);
    //new TCanvas("cGraph");
    
    //gWaveform->SetMarkerStyle(20);
    gWaveform->Draw();

    return;
}
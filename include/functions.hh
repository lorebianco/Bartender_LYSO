#ifndef FUNCTIONS_HH
#define FUNCTIONS_HH

#include <TCanvas.h>

#include "globals.hh"

extern TH3D *hAll;
extern TRandom3 *randNoise;


void SetDistros(const char *dataFilename, Double_t cutChargeMin = DBL_MIN, Double_t cutChargeMax = DBL_MAX);

Double_t Add_Noise(Double_t sigma);

Double_t Wave_OnePhel(Double_t x, Double_t A, Double_t tau_rise, Double_t tau_dec, Int_t timePhel);


#endif
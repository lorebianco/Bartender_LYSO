/**
 * @file SiPM.hh
 * @brief Declaration of class SiPM
 */
#ifndef SIPM_HH
#define SIPM_HH

#include <iostream>
#include <fstream>
#include <string>
#include <cfloat>

#include <TFile.h>
#include <TGraph.h>
#include <TMath.h>
#include <TSystem.h>
#include <TH3D.h>
#include <TTree.h>

using namespace std;

extern TH3D *hAll;


/// @brief Class for storing MPPC/SiPM settings for waveform generation.
class SiPM
{
public:
    
    SiPM(const char* sipmFilename);
    ~SiPM();

    /**
     * @fn SetDistros(Double_t cutChargeMin = DBL_MIN, Double_t cutChargeMax = DBL_MAX)
     * @brief Sets the 3D histogram @ref hAll
     *
     * This function takes all the best-fit parameters (\f$A \f$, \f$ \tau_{RISE} \f$, \f$ \tau_{DEC} \f$) obtained from the fitted I-Phel data waveforms and populates the histogram @ref hAll, from which sampling will occur. It specifically considers entries within the charge range [cutChargeMin, cutChargeMax] and with a converged fit status.
     *
     * @param cutChargeMin minimum cut for the my_charge distribution.
     * @param cutChargeMax maximum cut for the my_charge distribution.
     */
    void SetParsDistro(Double_t cutChargeMin = DBL_MIN, Double_t cutChargeMax = DBL_MAX);

private:
    string fBrand;
    string fTypeNo;
    Float_t fPixelPitch;
    Float_t fPhotosensitiveAreaWidth;
    Float_t fPhotosensitiveAreaHeight;
    Float_t fFillFactor;
    Float_t fPDE;
    Float_t fV;
    Float_t fT;
    Double_t fR_shaper;
    Float_t fGain;
    string fInputFilename;

    string extract_value(const string& line, const string& keyword);
    void ReadSiPMFile(const char* filename);
};


#endif
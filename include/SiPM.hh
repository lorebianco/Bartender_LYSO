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
    /**
     * @brief Constructur of the class. It takes as input parameter the name of the detector mac file and calls @ref ReadSiPMFile().
     */
    SiPM(const char* sipmFilename);

    /**
     * @brief Destructor of the class.
     */
    ~SiPM();

    /**
     * @fn SetParsDistro(Double_t cutChargeMin = DBL_MIN, Double_t cutChargeMax = DBL_MAX)
     * @brief Sets the 3D histogram @ref hAll
     *
     * This function takes all the best-fit parameters (\f$A \f$, \f$ \tau_{RISE} \f$, \f$ \tau_{DEC} \f$) obtained from the fitted I-Phel data waveforms and populates the histogram @ref hAll, from which sampling will occur. It specifically considers entries within the charge range [cutChargeMin, cutChargeMax] and with a converged fit status.
     *
     * @param cutChargeMin minimum cut for the my_charge distribution.
     * @param cutChargeMax maximum cut for the my_charge distribution.
     */
    void SetParsDistro(Double_t cutChargeMin = DBL_MIN, Double_t cutChargeMax = DBL_MAX);

    string GetBrand() { return fBrand;} ///< Returns @ref fBrand
    string GetTypeNo() { return fTypeNo;}   ///< Returns @ref fTypeNo
    Float_t GetPixelPitch() { return fPixelPitch;}  ///< Returns @ref fPixelPitch
    Float_t GetPhotosensitiveAreaWidth() { return fPhotosensitiveAreaWidth;}    ///< Returns @ref fPhotosensitiveAreaWidth
    Float_t GetPhotosensitiveAreaHeight() { return fPhotosensitiveAreaHeight;}  ///< Returns @ref fPhotosensitiveAreaHeight
    Float_t GetFillFactor() { return fFillFactor;}  ///< Returns @ref fFillFactor
    Float_t GetPDE() { return fPDE;}    ///< Returns @ref fPDE
    Float_t GetVoltage() { return fV;}  ///< Returns @ref fV
    Float_t GetTemperature() { return fT;}  ///< Returns @ref fT
    Double_t GetRShaper() { return fR_shaper;}  ///< Returns @ref fR_shaper
    Float_t GetGain() { return fGain;}  ///< Returns @ref fGain

private:
    string fBrand; ///< Manufacturer's brand of SiPM
    string fTypeNo; ///< Type number of the SiPM
    Float_t fPixelPitch; ///< Pixel Pitch of the SiPM
    Float_t fPhotosensitiveAreaWidth; ///< Width of the photosensitive active area of the SiPM
    Float_t fPhotosensitiveAreaHeight; ///< Height of the photosensitive active area of the SiPM
    Float_t fFillFactor; ///< Fill Factor of the SiPM
    Float_t fPDE; ///< Photon Detection Efficiency of the SiPM
    Float_t fV; ///< Supply Voltage [V] for the environment working point
    Float_t fT; ///< Temperature [°C] for the environment working point
    Double_t fR_shaper; ///< Value [Ohm] of the resistance of the shaper
    Float_t fGain; ///< Gain of the amplification stage
    string fInputFilename; ///< Name of the txt file of the best fit parameters data. See the introduction for more details about the file format


    /**
     * @fn string extract_value(const string& line, const string& keyword)
     * @brief Extracts a value from a line using a specific keyword.
     *
     * @param line The line from which to extract the value.
     * @param keyword The keyword indicating where the value is located in the line.
     * @return The extracted value as a string.
     */
    string extract_value(const string& line, const string& keyword);


    /**
     * @fn void ReadSiPMFile(const char* filename)
     * @brief Reads and processes a specific file to populate private members of the SiPM class.
     *
     * Reads the given file (filename) and loads relevant information into the private members of the SiPM class,
     * such as Brand, Type Number, Pixel Pitch, Photosensitive Area dimensions, Fill Factor, Photon Detection Efficiency,
     * Supply Voltage (V), Temperature (T), Resistance (R_shaper), Gain, and the name of the input file for best-fit waveform parameters.
     *
     * @param filename The name of the file (mac file) to be processed.
     */
    void ReadSiPMFile(const char* filename);

};


#endif
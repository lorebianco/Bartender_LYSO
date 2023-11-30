/**
 * @file run.hh
 * @brief Declaration of class Run
 */
#ifndef RUN_HH
#define RUN_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <cfloat>
#include <string>
#include <regex>

#include <TFile.h>
#include <TGraph.h>
#include <TMath.h>
#include <TSystem.h>
#include <TCanvas.h>

#include "globals.hh"

extern TH3D *hAll;

/// @brief Class for managing waveform construction for all events and channels.
class Run
{
public:
    /** 
     * @brief Constructor of the class.
     *
     * It takes as argument the number of events in the run and initializes the two waveform containers @ref fFront and @ref fBack calling @ref InitializeFrontWaveforms() and @ref InitializeBackWaveforms().
     *
     */
    Run(Int_t events);
    
    
    /**
     * @brief Destructor of the class.
     */
    ~Run();


    /**
     * @fn SetFrontWaveform(Int_t event, Int_t channel, Double_t start)
     * @brief Method to add a I-Phel waveform to the corresponding event and channel of the Front-Detector
     *
     * This function samples parameters (\f$A \f$, \f$ \tau_{RISE} \f$, \f$ \tau_{DEC} \f$) from @ref hAll, evaluates @ref Wave_OnePhel() in each bin, and finally sums it at the correct event and channel indices of @ref fFront.
     *
     * @param event Event index
     * @param channel Channel index
     * @param start Scintillation photon arrival time, provided to @ref Wave_OnePhel() as the timePhel (\f$ t_{phel} \f$) input parameter
     */
    void SetFrontWaveform(Int_t event, Int_t channel, Double_t start);


    /**
     * @fn SetBackWaveform(Int_t event, Int_t channel, Double_t start)
     * @brief Method to add a I-Phel waveform to the corresponding event and channel of the Back-Detector
     *
     * This function samples parameters (\f$A \f$, \f$ \tau_{RISE} \f$, \f$ \tau_{DEC} \f$) from @ref hAll, evaluates @ref Wave_OnePhel() in each bin, and finally sums it at the correct event and channel indices of @ref fBack.
     *
     * @param event Event index
     * @param channel Channel index
     * @param start Scintillation photon arrival time, provided to @ref Wave_OnePhel() as the timePhel (\f$ t_{phel} \f$) input parameter
     */
    void SetBackWaveform(Int_t event, Int_t channel, Double_t start);

    
    /** 
     * @fn void DrawFront(Int_t event, Int_t channel)
     * @brief Saves the TGraph of the selected waveform from the Front-Detector as a PNG image
     *
     * Note: This method is intended solely for hard-coded checks and debugging purposes.
     */
    void DrawFront(Int_t event, Int_t channel);
    
    
    /** 
     * @fn void DrawBack(Int_t event, Int_t channel)
     * @brief Saves the TGraph of the selected waveform from the Back-Detector as a PNG image
     *
     * Note: This method is intended solely for hard-coded checks and debugging purposes.
     */
    void DrawBack(Int_t event, Int_t channel);
    

    /**
     * @fn void SaveRun(const char* inputFilename)
     * @brief Saves all the samples from @ref fFront and @ref fBack into a text file.
     *
     * This method is designed to save the data samples contained in the objects @ref fFront and @ref fBack into a text file. The file naming convention is based on the RunID (XXXX) extracted from the MC input file. The output file is named as "BarID_XXXX.txt". Refer to the introduction for details about the file format.
     *
     * @param inputFilename The name of the MC input file used to extract the RunID (XXXX) for the output file naming.
     */
    void SaveRun(const char* inputFilename);



private:
    Int_t EVENTS; ///< Number of events in the run
    
    Double_t*** fFront; ///< Container for Front-Detector waveforms: a 3-dimensional matrix with indices for event, channel, and bin.
    
    Double_t*** fBack;  ///< Container for Back-Detector waveforms: a 3-dimensional matrix with indices for event, channel, and bin.

    TRandom3 *fRandPars = new TRandom3(0); ///< Random generator for @ref SetFrontWaveform() and @ref SetBackWaveform()

    TRandom3 *fRandNoise = new TRandom3(0); ///< Random generator for @ref Add_Noise()

    
    /**
     * @fn void InitializeFrontWaveforms()
     * @brief Method to initialize the entire @ref fFront with a noise baseline
     *
     * This function fills every bin in every channel and event with @ref Add_Noise()
     */
    void InitializeFrontWaveforms();
    

    /**
     * @fn void InitializeBackWaveforms()
     * @brief Method to initialize the entire @ref fBack with a noise baseline
     *
     * This function fills every bin in every channel and event with @ref Add_Noise()
     */
    void InitializeBackWaveforms();


    /**
     * @fn Double_t Add_Noise(Double_t sigma)
     * @brief Returns the value of the noise.
     *
     * This function provides a random value sampled from a normal distribution \f$ N(0, \sigma) \f$. It's important to note that the value of \f$ \sigma \f$ should be derived from experimental data, based on the pedestal's distribution.
     */
    Double_t Add_Noise(Double_t sigma);


    /**
     * @fn Double_t Wave_OnePhel(Double_t t, Double_t A, Double_t tau_rise, Double_t tau_dec, Int_t timePhel)
     * @brief Returns the value at t of the analytical form of the One Photo-Electron waveform.
     *
     * \f[
     * \text{wave}(t) = -A \Bigg( \exp \Bigg( -\frac{t - t_{phel}}{\tau_{\text{RISE}}} \Bigg) - \exp \Bigg( -\frac{t - t_{phel}}{\tau_{\text{DEC}}} \Bigg) \Bigg)  \theta( t - t_{phel} )
     * \f] 
     */
    Double_t Wave_OnePhel(Double_t t, Double_t A, Double_t tau_rise, Double_t tau_dec, Int_t timePhel);

};

#endif
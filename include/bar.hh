/**
 * @file bar.hh
 * @brief Declaration of the class Bar
 */ 
#ifndef BAR_HH
#define BAR_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <cfloat>
#include <string>
#include <regex>

#include <TH3D.h>
#include <TRandom3.h>
#include <TTree.h>
#include <TMath.h>

#include "globals.hh"

/**
 * @brief Class for managing waveform construction for all events and channels.
 */
class Bar
{
public:
    /** 
     * @brief Constructor of the class.
     *
     * It takes as argument the MC-filename and sets the simulation ID (@ref fID)
     *
     * @param inputFilename MC-filename used in the simulation.
     */
    Bar(const char* inputFilename);
    /**
     * @brief Destructor of the class.
     */
    ~Bar();

    /**
     * @brief Sets the 3D histogram @ref hPars
     *
     * This function takes all the best-fit parameters (\f$A \f$, \f$
     * \tau_{RISE} \f$, \f$ \tau_{DEC} \f$) obtained from the fitted I-Phel
     * data waveforms, defines and populates the histogram @ref hPars (from
     * which sampling will occur) with settings @ref fHisto_A, @ref
     * fHisto_Tau_rise and @ref fHisto_Tau_dec. It specifically considers
     * entries within the charge range [@ref fChargeCuts[0],
     * @ref fChargeCuts[1]] and with a converged fit status.
     */
    void SetParsDistro();
    /**
     * @brief Method to initialize the entire @ref fFront and @ref fBack with a
     * noise baseline.
     *
     * This function sets the number of events in the run (@ref EVENTS =
     * newEvents) and fills every bin in every channel and event with @ref
     * Add_Noise()
     *
     * @param newEvents Number of events in the MC-Simulation
     */
    void InitializeBaselines(Int_t newEvents);
    /**
     * @brief Method to add a I-Phel waveform to the corresponding event and
     * channel of the Front-Detector
     *
     * This function samples parameters (\f$A \f$, \f$ \tau_{RISE} \f$, \f$
     * \tau_{DEC} \f$) from @ref hPars, evaluates @ref Wave_OnePhel() in each
     * bin, and finally sums it at the correct event and channel indices of
     * @ref fFront.
     *
     * @param event Event index
     * @param channel Channel index
     * @param start Scintillation photon arrival time, provided to @ref
     * Wave_OnePhel() as the timePhel (\f$ t_{phel} \f$) input parameter
     */
    void SetFrontWaveform(Int_t event, Int_t channel, Double_t start);
    /**
     * @brief Method to add a I-Phel waveform to the corresponding event and channel of the Back-Detector
     *
     * This function samples parameters (\f$A \f$, \f$ \tau_{RISE} \f$, \f$
     * \tau_{DEC} \f$) from @ref hPars, evaluates @ref Wave_OnePhel() in each
     * bin, and finally sums it at the correct event and channel indices of
     * @ref fBack.
     *
     * @param event Event index
     * @param channel Channel index
     * @param start Scintillation photon arrival time, provided to @ref
     * Wave_OnePhel() as the timePhel (\f$ t_{phel} \f$) input parameter
     */
    void SetBackWaveform(Int_t event, Int_t channel, Double_t start);
    /**
     * @brief Saves all the samples from @ref fFront and @ref fBack into a text
     * file.
     *
     * This method is designed to save the data samples contained in the
     * objects @ref fFront and @ref fBack into a text file. The file naming
     * convention is based on the RunID @ref fID extracted from the MC input
     * file. The output file is named as "BarID_fID.txt". Refer to the
     * introduction for details about the file format.
     */
    void SaveBar();

    inline void SetSigmaNoise(Double_t newSigmaNoise) { fSigmaNoise = newSigmaNoise; } /**< @brief Set @ref fSigmaNoise, the noise of the DAQ. */
    inline void SetInputFilename(std::string newInputFilename) { fInputFilename = newInputFilename; } /**< @brief Set the name of the text file of the best fit parameters data. */
    /**
     * @brief Set the cuts in the charge spectrum of input best fit parameters
     * data.
     */
    inline void SetChargeCuts(Double_t min, Double_t max)
    {
        fChargeCuts[0] = min;
        fChargeCuts[1] = max;
    }
    /**
     * @brief Set the number of bins, the lower and the upper limit related to
     * parameter A for the histogram @ref hPars. 
     */
    inline void SetHisto_A(Double_t nbins, Double_t min, Double_t max)
    {
        fHisto_A[0] = TMath::Nint(nbins);
        fHisto_A[1] = min;
        fHisto_A[2] = max;
    }
    /**
     * @brief Set the number of bins, the lower and the upper limit related to
     * parameter Tau_rise for the histogram @ref hPars. 
     */
    inline void SetHisto_Tau_rise(Double_t nbins, Double_t min, Double_t max)
    {
        fHisto_Tau_rise[0] = TMath::Nint(nbins);
        fHisto_Tau_rise[1] = min;
        fHisto_Tau_rise[2] = max;
    }
    /**
     * @brief Set the number of bins, the lower and the upper limit related to
     * parameter Tau_dec for the histogram @ref hPars. 
     */
    inline void SetHisto_Tau_dec(Double_t nbins, Double_t min, Double_t max)
    {
        fHisto_Tau_dec[0] = TMath::Nint(nbins);
        fHisto_Tau_dec[1] = min;
        fHisto_Tau_dec[2] = max;
    }

    inline Int_t GetEvents() const { return EVENTS; } /**< @brief Returns the number of events in the run. */
    inline Int_t GetID() const { return fID; } /**< @brief Returns the ID of the Monte Carlo. */
private:
    Int_t EVENTS; /**< @brief Number of events in the run */
    Int_t fID; /**< @brief Run ID of the Monte Carlo */

    Double_t*** fFront; /**< @brief Container for Front-Detector waveforms: a 3-dimensional matrix with indices for event, channel, and bin. */  
    Double_t*** fBack;  /**< @brief Container for Back-Detector waveforms: a 3-dimensional matrix with indices for event, channel, and bin. */

    TH3D *hPars; /**< @brief 3D Histogram of One-Phel waveform parameters from which sampling will occur */
    
    TRandom3 *fRandPars = new TRandom3(0); /**< @brief Random generator for @ref SetFrontWaveform() and @ref SetBackWaveform() */
    TRandom3 *fRandNoise = new TRandom3(0); /**< @brief Random generator for @ref Add_Noise() */

    Double_t fSigmaNoise; /**< @brief Noise of the DAQ, evaluated as the stDev of the pedestal distribution */

    std::string fInputFilename; /**< @brief Name of the txt file of the best fit parameters data. See the introduction for more details about the file format */
    Double_t fChargeCuts[2]; /**< @brief Cuts in the charge spectrum of input best fit parameters data; [0] represents the minimum, [1] represents the maximum. */
    Double_t fHisto_A[3]; /**< @brief Settings for histogram @ref hPars related to parameter A: [0] for number of bins, [1] for the lower limit, [2] for the upper limit. */
    Double_t fHisto_Tau_rise[3]; /**< @brief Settings for histogram @ref hPars related to parameter Tau_rise: [0] for number of bins, [1] for the lower limit, [2] for the upper limit. */
    Double_t fHisto_Tau_dec[3]; /**< @brief Settings for histogram @ref hPars related to parameter Tau_dec: [0] for number of bins, [1] for the lower limit, [2] for the upper limit. */ 

    /**
     * @brief Returns the value of the noise.
     *
     * This function provides a random value sampled from a normal distribution
     * \f$N\f$(0, @ref sigmaNoise). It's important to note that the value of
     * \f$ \sigma \f$ should be derived from experimental data, based on the
     * pedestal's distribution.
     */
    inline Double_t Add_Noise() { return fRandNoise->Gaus(0, fSigmaNoise); }
    /**
     * @brief Returns the value at t of the analytical form of the One Photo-Electron waveform.
     *
     * \f[
     * \text{wave}(t) = -A \Bigg( \exp \Bigg( -\frac{t - t_{phel}}{\tau_{\text{RISE}}} \Bigg) - \exp \Bigg( -\frac{t - t_{phel}}{\tau_{\text{DEC}}} \Bigg) \Bigg)  \theta( t - t_{phel} )
     * \f] 
     */
    Double_t Wave_OnePhel(Double_t t, Double_t A, Double_t tau_rise, Double_t tau_dec, Int_t timePhel);

};

#endif  // BAR_HH
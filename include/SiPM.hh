/**
 * @file SiPM.hh
 * @brief Definition of struct SiPM
 */
#ifndef SIPM_HH
#define SIPM_HH


/// @brief Struct for storing MPPC/SiPM settings for waveform generation.
struct SiPM
{
    string fBrand; ///< Manufacturer's brand of SiPM
    string fTypeNo; ///< Type number of the SiPM
    
    Float_t fV; ///< Supply Voltage [V] for the environment working point
    Float_t fT; ///< Temperature [°C] for the environment working point
    Double_t fR_shaper; ///< Value [Ohm] of the resistance of the shaper
    Float_t fGain; ///< Gain of the amplification stage
    Double_t fSigmaNoise; ///< Noise of the DAQ, evaluated as the stDev of the pedestal distribution
};


#endif //SIPM_HH
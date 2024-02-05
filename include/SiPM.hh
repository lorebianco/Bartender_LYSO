/**
 * @file SiPM.hh
 * @brief Definition of the struct SiPM
 */
#ifndef SIPM_HH
#define SIPM_HH

#include <string>

/**
 * @brief Struct for storing MPPC/SiPM settings for waveform generation.
 */
struct SiPM
{
    std::string fBrand; /**< @brief Manufacturer's brand of SiPM */
    std::string fTypeNo; /**< @brief Type number of the SiPM */
    
    Float_t fV; /**< @brief Supply Voltage [V] for the environment working point */
    Float_t fT; /**< @brief Temperature [°C] for the environment working point */
    Double_t fR_shaper; /**< @brief Value [Ohm] of the resistance of the shaper */
    Float_t fGain; /**< @brief Gain of the amplification stage */
    Double_t fSigmaNoise; /**< @brief Noise of the DAQ, evaluated as the stDev of the pedestal distribution */
};


#endif  // SIPM_HH
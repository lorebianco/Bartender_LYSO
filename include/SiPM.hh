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
};


#endif  // SIPM_HH
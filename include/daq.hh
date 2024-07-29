/**
 * @file DAQ.hh
 * @brief Definition of the struct DAQ
 */
#ifndef DAQ_HH
#define DAQ_HH

#include <TMath.h>


struct DAQ
{
    Float_t fSamplingSpeed; /**< @brief Sampling speed [GSPS] of the DAQ */
    Double_t fR_shaper; /**< @brief Value [Ohm] of the resistance of the shaper */
    Float_t fGain; /**< @brief Gain of the amplification stage */
    Float_t fGainOfTemplate;
    Double_t fFactorOfGainConversion;
    Double_t fSigmaNoise; /**< @brief Noise of the DAQ, evaluated as the stDev of the pedestal distribution */

    inline void ComputeFactorOfGainConversion() { fFactorOfGainConversion = TMath::Power(10, (1./20)*(fGain - fGainOfTemplate)); }
};


#endif  // DAQ_HH
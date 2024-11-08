/**
 * @file DAQ.hh
 * @brief Definition of the struct DAQ
 */
#ifndef DAQ_HH
#define DAQ_HH

#include <TMath.h>
#include <TRandom3.h>

class DAQ
{
  public:
    DAQ() { binRand = new TRandom3(0); };
    ~DAQ() { delete binRand; };

    // Sampling
    Float_t fSamplingSpeed; /**< @brief Sampling speed [GSPS] of the DAQ */
    Float_t fSamplingSpeed_Template;
    Bool_t fIsBinSizeConstant;
    Double_t fSigmaBinSize;
    TRandom3 *binRand;
    
    // Amplification
    Float_t fGain; /**< @brief Gain of the amplification stage */
    Float_t fGain_Template;
    inline Double_t ComputeFactorOfGainConversion() { return TMath::Power(10, (1./20)*(fGain - fGain_Template)); }
    
    // Shaping
    Bool_t fIsShaping;
    Double_t fTau_shaping;
    Double_t fR_shaper_Template; /**< @brief Value [Ohm] of the resistance of the shaper */
    Double_t fSigmaNoise; /**< @brief Noise of the DAQ, evaluated as the stDev of the pedestal distribution */
};


#endif  // DAQ_HH
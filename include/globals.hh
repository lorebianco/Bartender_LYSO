/**
 * @file globals.hh
 * @brief Definition of global parameters shared among classes and functions.
 */
#ifndef GLOBALS_HH
#define GLOBALS_HH

using namespace std;
using namespace TMath;

#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH3D.h>
#include <TMath.h>
#include <TString.h>
#include <TSystem.h>
#include <TTree.h>
#include <TRandom3.h>

#define CHANNELS 115    ///< Number of channels of the detectors
#define SAMPLINGS 1024  ///< Number of samplings for one waveform

#define ZERO_TIME_BIN 450   ///< Delay of all waveforms in the [0, 1023] bins window

#define SIGMA_NOISE 0.02    ///< StDev of the pedestal distribution


#endif
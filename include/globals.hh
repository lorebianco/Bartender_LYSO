/**
 * @file globals.hh
 * @brief Definition of global constant values for the simulation.
 */
#ifndef GLOBALS_HH
#define GLOBALS_HH


constexpr Int_t CHANNELS = 115; /**< @brief Number of channels of the detectors */
constexpr Int_t SAMPLINGS = 1024; /**< @brief Number of samplings for one waveform */

constexpr Int_t ZERO_TIME_BIN = 450; /**< @brief Delay of all waveforms in the [0, 1023] bins window */


#endif  // GLOBALS_HH
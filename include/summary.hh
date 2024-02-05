/**
 * @file summary.hh
 * @brief Definition of the function @ref Bartender_Summary().
 */
#ifndef SUMMARY_HH
#define SUMMARY_HH

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

#include "bar.hh"
#include "SiPM.hh"

/**
 * @brief Writes the summary of the Bartender to a text file, updating it at
 * the end of every simulation.
 * The summary includes:
 * - Bartender ID (equal to the ID of the MC_LYSO), date, username and duration;
 * - Settings related to the MPPC working point.
 *
 *
 * @param duration The duration (in seconds) of the simulation
 * @param bar The Bar class instance pointer
 * @param sipm The SiPM struct instance pointer
 */
void Bartender_Summary(Double_t duration, Bar* bar, SiPM* sipm);


#endif  // SUMMARY_HH
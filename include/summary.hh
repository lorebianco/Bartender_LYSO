#ifndef SUMMARY_HH
#define SUMMARY_HH

#include <string>
#include <fstream>
#include <iostream>
#include <ctime>
#include <unistd.h>

/**
 * @brief Generates a summary file with the provided information about the simulation.
 *
 * @param macrofile Path to the macro file containing configuration details.
 * @param MCID Unique identifier for the Monte Carlo simulation.
 * @param duration Duration of the simulation in seconds.
 */
void Bartender_Summary(const std::string& macrofile, int MCID, double duration);

#endif // SUMMARY_HH

/**
 * @file configure.hh
 * @brief Declaration of the function @ref Bartender_Configure() (and the
 * auxiliary function @ref extract_value())
 */
#ifndef CONFIGURE_HH
#define CONFIGURE_HH

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cfloat>

#include "bar.hh"
#include "SiPM.hh"

/**
 * @brief Extracts a value from a line using a specific keyword.
 *
 * @param line The line from which to extract the value.
 * @param keyword The keyword indicating where the value is located in the line.
 * @return The extracted value as a string.
 */
std::string extract_value(const std::string& line, const std::string& keyword);


/**
 * @brief Reads and processes a specific file to populate members of the Bar
 * class and SiPM struct.
 *
 * Reads the given file (filename) and loads relevant information into the
 * members of @ref Bar and @ref SiPM, such as Brand, Type Number, Supply
 * Voltage (V), Temperature (T), Resistance (R_shaper), Gain, the name of the
 * input file for best-fit waveform parameters and the @hPars settings.
 *
 * @param filename The name of the file (mac file) to be processed.
 * @param bar Bar class pointer 
 * @param sipm SiPM struct pointer
 */
void Bartender_Configure(const char* filename, Bar* bar, SiPM* sipm);


#endif  // CONFIGURE_HH
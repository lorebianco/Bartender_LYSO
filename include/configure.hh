/**
 * @file configure.hh
 * @brief Definition of the function @ref Bartender_Configure() (and the auxiliary function @ref extract_value()), used to configure all the settings for the Bartender simulation
 */

#ifndef CONFIGURE_HH
#define CONFIGURE_HH

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cfloat>

using namespace std;


#include "bar.hh"
#include "SiPM.hh"


/**
 * @fn string extract_value(const string& line, const string& keyword)
 * @brief Extracts a value from a line using a specific keyword.
 *
 * @param line The line from which to extract the value.
 * @param keyword The keyword indicating where the value is located in the line.
 * @return The extracted value as a string.
 */
string extract_value(const string& line, const string& keyword)
{
    size_t start = line.find(keyword);
    if(start != string::npos)
    {
        start += keyword.length();
        string value = line.substr(start);

        // Remove spaces before string
        size_t first_not_space = value.find_first_not_of(" \t");
        if(first_not_space != string::npos)
        {
            value = value.substr(first_not_space);
        }

        // Remove spaces after string
        size_t last_not_space = value.find_last_not_of(" \t");
        if(last_not_space != string::npos)
        {
            value = value.substr(0, last_not_space + 1);
        }

        return value;
    }
    return "";
}



/**
 * @fn void Bartender_Configure(const char* filename, Bar* bar, SiPM* sipm)
 * @brief Reads and processes a specific file to populate members of the Bar class and SiPM struct.
 *
 * Reads the given file (filename) and loads relevant information into the members of @ref Bar and @ref SiPM,
 * such as Brand, Type Number, Supply Voltage (V), Temperature (T), Resistance (R_shaper), Gain, the name of the input file for best-fit waveform parameters and the @hPars settings.
 *
 * @param filename The name of the file (mac file) to be processed.
 * @param bar Bar class pointer 
 * @param sipm SiPM struct pointer
 */
void Bartender_Configure(const char* filename, Bar* bar, SiPM* sipm)
{
    ifstream file(filename);
    if(!file.is_open())
    {
        cerr << "Can't open the sipm file!" << endl;
        return;
    }

    string line;
    while (getline(file, line))
    {
        if(line.find("Brand:") != string::npos)
        {
            sipm->fBrand = extract_value(line, "Brand:");
        }
        else if(line.find("TypeNo:") != string::npos)
        {
            sipm->fTypeNo = extract_value(line, "TypeNo:");
        }
        else if(line.find("V =") != string::npos)
        {
            sipm->fV = stof(extract_value(line, "V ="));
        }
        else if(line.find("T =") != string::npos)
        {
            sipm->fT = stof(extract_value(line, "T ="));
        }
        else if(line.find("R_shaper =") != string::npos)
        {
            sipm->fR_shaper = stod(extract_value(line, "R_shaper ="));
        }
        else if(line.find("Gain =") != string::npos)
        {
            sipm->fGain = stof(extract_value(line, "Gain ="));
        }
        else if(line.find("Noise (sigma) =") != string::npos)
        {
            bar->SetSigmaNoise(stof(extract_value(line, "Noise (sigma) =")));
            sipm->fSigmaNoise = stof(extract_value(line, "Noise (sigma) ="));
        }
        else if(line.find("PathToFile:") != string::npos)
        {
            bar->SetInputFilename(extract_value(line, "PathToFile:"));
        }
        else if(line.find("Charge cuts:") != string::npos)
        {
            string data = extract_value(line, "Charge cuts:");
            istringstream iss(data);
            Double_t charge_min, charge_max;
            if(iss >> charge_min >> charge_max)
            {
                bar->SetChargeCuts(charge_min, charge_max);
            }
        }
        else if(line.find("A histo:") != string::npos)
        {
            string data = extract_value(line, "A histo:");
            istringstream iss(data);
            Double_t nbins, hist_min, hist_max;
            if(iss >> nbins >> hist_min >> hist_max)
            {
                bar->SetHisto_A(nbins, hist_min, hist_max);
            }
        }
        else if(line.find("Tau_rise histo:") != string::npos)
        {
            string data = extract_value(line, "Tau_rise histo:");
            istringstream iss(data);
            Double_t nbins, hist_min, hist_max;
            if(iss >> nbins >> hist_min >> hist_max)
            {
                bar->SetHisto_Tau_rise(nbins, hist_min, hist_max);
            }
        }
        else if(line.find("Tau_dec histo:") != string::npos)
        {
            string data = extract_value(line, "Tau_dec histo:");
            istringstream iss(data);
            Double_t nbins, hist_min, hist_max;
            if(iss >> nbins >> hist_min >> hist_max)
            {
                bar->SetHisto_Tau_dec(nbins, hist_min, hist_max);
            }
        }

    }

    file.close();
}

#endif  //CONFIGURE_HH
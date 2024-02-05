/**
 * @file configure.cc
 * @brief Definition of the function @ref Bartender_Configure() (and the auxiliary function @ref extract_value())
 */
#include "configure.hh"

using namespace std;


string extract_value(const string& line, const string& keyword)
{
    // Find the position of the keyword in the line
    size_t start = line.find(keyword);

    // If the keyword is found, extract the value following it
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

    // Return an empty string if the keyword is not found
    return "";
}



void Bartender_Configure(const char* filename, Bar* bar, SiPM* sipm)
{
    // Open the mac file
    ifstream file(filename);
    if(!file.is_open())
    {
        cerr << "Can't open the sipm file!" << endl;
        return;
    }

    // Read each line from the file
    string line;
    while (getline(file, line))
    {
        // Check for different parameters in each line and update corresponding values    
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

    // Close the file
    file.close();
}
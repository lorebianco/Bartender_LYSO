/**
 * @file summary.cc
 * @brief Definition of the function @ref Bartender_Summary()
 */
#include "summary.hh"

using namespace std;


void Bartender_Summary(Double_t duration, Bar* bar, SiPM* sipm)
{
    // Open the output file in append mode
    ofstream file("Bartender_Summaries.txt", ios::app);

    if(file.is_open())
    {
        // Print general information about the simulation (ID, date, username, duration)
        file << "Bartender serial number (MCID): " << bar->GetID() << "\n";
        file << endl;
        time_t now = time(0);
        tm* current_time = localtime(&now);
        file << "Date: " << asctime(current_time);
        file << "User Name: " << getlogin() << endl;
        file << "Duration of the simulation: " << duration << " s" << endl;
        file << endl;

        // Print information about the SiPM and its working point
        file << "SiPM: " << sipm->fBrand << " " << sipm->fTypeNo << "\n";
        file << "Voltage: " << sipm->fV << " V\n";
        file << "Temperature: " << sipm->fT << "°C\n";
        file << "R_shaper: " << sipm->fR_shaper << " Ohm\n";
        file << "Gain: " << sipm->fGain << " db\n";
        file << "Noise (sigma): " << sipm->fSigmaNoise << " V\n\n";

        // Print the number of events
        file << "Number of events: " << bar->GetEvents() << "\n\n";
        file << "########################################################\n\n";

        file.close();
    }
    else
    {
        cerr << "Can't open summary file!" << endl;
    }
}

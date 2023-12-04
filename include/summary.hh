/**
 * @file summary.hh
 * @brief Definition of the function @ref Bartender_Summary(), used to save the relevant infos of the Bartender simulation
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

using namespace std;

extern Double_t sigmaNoise;


void Bartender_Summary(Double_t duration, Bar* bar, SiPM* sipm)
{
    ofstream file("Bartender_Summaries.txt", ios::app);

    if(file.is_open())
    {
        file << "Bartender serial number (MCID): " << bar->GetID() << "\n";
        file << endl;
        time_t now = time(0);
        tm* current_time = localtime(&now);
        file << "Date: " << asctime(current_time);
        file << "User Name: " << getlogin() << endl;
        file << "Duration of the simulation: " << duration << endl;
        file << endl;
        file << "SiPM: " << sipm->GetBrand() << " " << sipm->GetTypeNo() << "\n";
        file << "Voltage: " << sipm->GetVoltage() << " V\n";
        file << "Temperature: " << sipm->GetTemperature() << "°C\n";
        file << "R_shaper: " << sipm->GetRShaper() << " Ohm\n";
        file << "Gain: " << sipm->GetGain() << " db\n";
        file << "Noise (sigma): " << sigmaNoise << " V\n\n";

        file << "Number of events: " << bar->GetEvents() << "\n\n";
        file << "########################################################\n\n";

        file.close();
    }
    else
    {
        cout << "Can't open summary file!" << endl;
    }
}


#endif

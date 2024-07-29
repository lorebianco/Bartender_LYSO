#include <cstdlib>
#include <thread>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <chrono>

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



void Bartender_Summary(string macrofile, int MCID, double duration)
{
    // Open the output file in append mode
    ofstream outfile("Bartender_summaries.txt", ios::app);

    // Print general information about the simulation (MCID, date, username, duration)
    outfile << "Bartender serial number (MCID): " << MCID << endl;
    outfile << endl;
    time_t now = time(0);
    tm* current_time = localtime(&now);
    outfile << "Date: " << asctime(current_time);
    outfile << "User Name: " << getlogin() << endl;
    outfile << "Duration of the simulation: " << duration << " s" << endl;
    outfile << endl;

    // Open the run macro file in read mode
    ifstream run_file(macrofile);
    if(!run_file)
    {
        cerr << "Can't open run macro file!" << endl;
        return;
    }

    string line;
    string brand, model;

    // Scan the run macro file
    while(getline(run_file, line))
    {
        // Trim leading whitespaces and tabs from each line
        line = line.substr(line.find_first_not_of(" \t"));

        // Find all the settings and write them on the output file
        if(line.find("Brand:") != string::npos)
        {
            brand = extract_value(line, "Brand:");
        }
        else if(line.find("TypeNo:") != string::npos)
        {
            model = extract_value(line, "TypeNo:");
            if(!brand.empty() && !model.empty())
            {
                outfile << "SiPM: " << brand << " " << model << endl;
            }
        }
        else if(line.find("V =") != string::npos)
        {
            string V_value = extract_value(line, "V =");
            if(!V_value.empty())
            {
                outfile << "Voltage: " << V_value << endl;
            }
        }
        else if(line.find("T =") != string::npos)
        {
            string T_value = extract_value(line, "T =");
            if(!T_value.empty())
            {
                outfile << "Temperature: " << T_value << endl;
            }
        }
        else if(line.find("Sampling speed =") != string::npos)
        {
            string sampling_value = extract_value(line, "Sampling speed =");
            if(!sampling_value.empty())
            {
                outfile << "Sampling speed: " << sampling_value << endl;
            }
        }
        else if(line.find("R_shaper =") != string::npos)
        {
            string R_value = extract_value(line, "R_shaper =");
            if(!R_value.empty())
            {
                outfile << "R_shaper: " << R_value << endl;
            }
        }
        else if(line.find("Gain =") != string::npos)
        {
            string gain_value = extract_value(line, "Gain =");
            if(!gain_value.empty())
            {
                outfile << "Gain: " << gain_value << endl;
            }
        }
        else if(line.find("Noise (sigma) =") != string::npos)
        {
            string noise_value = extract_value(line, "Noise (sigma) =");
            if(!noise_value.empty())
            {
                outfile << "Noise (sigma): " << noise_value << endl;
            }
        }
    }

    run_file.close();

    outfile << endl;
    outfile << "########################################################" << endl;
    outfile << endl;
}



void launchProcess(int MCID, const char* sipmFilename, int threadID)
{
    // Genera il nome del file di output basato sul threadID
    ostringstream mcFile;
    mcFile << "/home/lorenzo/MEG_Project/Simulazioni/MC_LYSO/build_mt/RootFiles/" << MCID << "/MCID_" << MCID << "_t" << threadID << ".root";

    // Costruisci il comando con il nome del file di output e il SiPM.mac
    ostringstream command;
    command << "./bartender " << mcFile.str() << " " << sipmFilename << " " << threadID;

    // Esegui il comando
    system(command.str().c_str());
}



int main(int argc, char** argv)
{
    if(argc < 3)
    {
        cerr << "Usage: " << argv[0] << " MCID sipmFilename [numFiles]" << endl;
        return 1;
    }

    // Get input files
    int MCID = stoi(argv[1]);
    const char *sipmFilename = argv[2];
    int numFiles = (argc > 3) ? stoi(argv[3]) : 16;

    auto start_chrono = chrono::high_resolution_clock::now();

    // Vettore per tenere traccia dei thread
    vector<thread> threads(numFiles);

    // Itera su tutti i file in input
    for (int i = 0; i < numFiles; ++i)
    {
        // Avvia il processo in un nuovo thread, passando l'ID del thread
        threads[i] = thread(launchProcess, MCID, sipmFilename, i);
    }

    // Attendi che tutti i thread terminino
    for (int i = 0; i < numFiles; ++i)
    {
        threads[i].join();
    }

    auto end_chrono = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_chrono - start_chrono;

    cout << "Duration: " << duration.count() << " s" << endl;

    Bartender_Summary(sipmFilename, MCID, duration.count());

    return 0;
}

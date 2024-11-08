#include <cstdlib>
#include <thread>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <chrono>

#include "summary.hh"


using namespace std;


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
    for(int i = 0; i < numFiles; i++)
    {
        // Avvia il processo in un nuovo thread, passando l'ID del thread
        threads[i] = thread(launchProcess, MCID, sipmFilename, i);
    }

    // Attendi che tutti i thread terminino
    for(int i = 0; i < numFiles; i++)
    {
        threads[i].join();
    }

    auto end_chrono = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_chrono - start_chrono;

    cout << "Duration: " << duration.count() << " s" << endl;

    Bartender_Summary(sipmFilename, MCID, duration.count());

    return 0;
}

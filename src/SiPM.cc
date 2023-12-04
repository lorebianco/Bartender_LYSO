/**
 * @file SiPM.cc
 * @brief Definition of class SiPM
 */
#include "SiPM.hh"


SiPM::SiPM(const char* sipmFilename)
{
    ReadSiPMFile(sipmFilename);
}



SiPM::~SiPM()
{}




void SiPM::SetParsDistro()
{
    Int_t status;
    Double_t my_charge, A, tau_rise, tau_dec;

    TTree *tree = new TTree("tree", "mytree");
    tree->ReadFile(fInputFilename.c_str());

    tree->SetBranchAddress("Status", &status);
    tree->SetBranchAddress("my_charge", &my_charge);
    tree->SetBranchAddress("A", &A);
    tree->SetBranchAddress("Tau_rise", &tau_rise);
    tree->SetBranchAddress("Tau_dec", &tau_dec);

    hAll = new TH3D("hAll", "Fitted All", fHisto_A[0], fHisto_A[1], fHisto_A[2], fHisto_Tau_rise[0], fHisto_Tau_rise[1], fHisto_Tau_rise[2], fHisto_Tau_dec[0], fHisto_Tau_dec[1], fHisto_Tau_dec[2]);

    for(Int_t i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);
        if(status==0 && my_charge >= fChargeCuts[0] && my_charge <= fChargeCuts[1])       
        {
            hAll->Fill(A, tau_rise, tau_dec);
        }
    }

    cout << "Distro setted with entries with charge in [" << fChargeCuts[0] << ", " << fChargeCuts[1] << "]" << endl;

    delete tree;
}




string SiPM::extract_value(const string& line, const string& keyword)
{
    size_t start = line.find(keyword);
    if(start != string::npos)
    {
        start += keyword.length();
        string value = line.substr(start);

        // Rimuovi gli spazi all'inizio della stringa
        size_t first_not_space = value.find_first_not_of(" \t");
        if(first_not_space != string::npos)
        {
            value = value.substr(first_not_space);
        }

        // Rimuovi gli spazi alla fine della stringa
        size_t last_not_space = value.find_last_not_of(" \t");
        if(last_not_space != string::npos)
        {
            value = value.substr(0, last_not_space + 1);
        }

        return value;
    }
    return "";
}




void SiPM::ReadSiPMFile(const char* filename)
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
            fBrand = extract_value(line, "Brand:");
        }
        else if(line.find("TypeNo:") != string::npos)
        {
            fTypeNo = extract_value(line, "TypeNo:");
        }
        else if(line.find("V =") != string::npos)
        {
            fV = stof(extract_value(line, "V ="));
        }
        else if(line.find("T =") != string::npos)
        {
            fT = stof(extract_value(line, "T ="));
        }
        else if(line.find("R_shaper =") != string::npos)
        {
            fR_shaper = stod(extract_value(line, "R_shaper ="));
        }
        else if(line.find("Gain =") != string::npos)
        {
            fGain = stof(extract_value(line, "Gain ="));
        }
        else if(line.find("Noise (sigma) =") != string::npos)
        {
            sigmaNoise = stof(extract_value(line, "Noise (sigma) ="));
        }
        else if(line.find("PathToFile:") != string::npos)
        {
            fInputFilename = extract_value(line, "PathToFile:");
        }
        else if(line.find("Charge cuts:") != string::npos)
        {
            string data = extract_value(line, "Charge cuts:");
            istringstream iss(data);
            Double_t charge_min, charge_max;
            if(iss >> charge_min >> charge_max)
            {
                fChargeCuts[0] = charge_min;
                fChargeCuts[1] = charge_max;
            }
        }
        else if(line.find("A histo:") != string::npos)
        {
            string data = extract_value(line, "A histo:");
            istringstream iss(data);
            Double_t nbins, hist_min, hist_max;
            if(iss >> nbins >> hist_min >> hist_max)
            {
                fHisto_A[0] = nbins;
                fHisto_A[1] = hist_min;
                fHisto_A[2] = hist_max;
            }
        }
        else if(line.find("Tau_rise histo:") != string::npos)
        {
            string data = extract_value(line, "Tau_rise histo:");
            istringstream iss(data);
            Double_t nbins, hist_min, hist_max;
            if(iss >> nbins >> hist_min >> hist_max)
            {
                fHisto_Tau_rise[0] = nbins;
                fHisto_Tau_rise[1] = hist_min;
                fHisto_Tau_rise[2] = hist_max;
            }
        }
        else if(line.find("Tau_dec histo:") != string::npos)
        {
            string data = extract_value(line, "Tau_dec histo:");
            istringstream iss(data);
            Double_t nbins, hist_min, hist_max;
            if(iss >> nbins >> hist_min >> hist_max)
            {
                fHisto_Tau_dec[0] = nbins;
                fHisto_Tau_dec[1] = hist_min;
                fHisto_Tau_dec[2] = hist_max;
            }
        }

    }

    file.close();
}
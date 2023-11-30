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




void SiPM::SetParsDistro(Double_t cutChargeMin, Double_t cutChargeMax)
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

    // Need to find a way to do this without hardcoding
    hAll = new TH3D("hAll", "Fitted All", 50 ,0, 0.6, 50, 0, 1.5, 50, 0, 11);

    for(Int_t i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);
        if(status==0 && my_charge >= cutChargeMin && my_charge <= cutChargeMax)       
        {
            hAll->Fill(A, tau_rise, tau_dec);
        }
    }

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
        else if(line.find("Pixel Pitch =") != string::npos)
        {
            string pixelPitchStr = extract_value(line, "Pixel Pitch =");
            fPixelPitch = stof(pixelPitchStr); // Converti la stringa in float
        }
        else if(line.find("Photosensitive Area =") != string::npos)
        {
            // Estrai la larghezza e l'altezza dell'area photosensibile e convertile in float
            string areaStr = extract_value(line, "Photosensitive Area =");
            size_t pos = areaStr.find('x');
            fPhotosensitiveAreaWidth = stof(areaStr.substr(0, pos));
            fPhotosensitiveAreaHeight = stof(areaStr.substr(pos + 1));
        }
        else if(line.find("Fill Factor =") != string::npos)
        {
            fFillFactor = stof(extract_value(line, "Fill Factor ="));
        }
        else if(line.find("PDE =") != string::npos)
        {
            fPDE = stof(extract_value(line, "PDE ="));
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
        else if(line.find("PathToFile:") != string::npos)
        {
            fInputFilename = extract_value(line, "PathToFile:");
        }
    }

    file.close();
}
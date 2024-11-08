#include "summary.hh"

// Helper function to extract values from macro file lines
std::string summary_extract_value(const std::string& line, const std::string& keyword)
{
    size_t start = line.find(keyword);
    if(start != std::string::npos && start == 0)
    {
        start += keyword.length();
        std::string value = line.substr(start);
        
        // Trim spaces
        size_t first_not_space = value.find_first_not_of(" \t");
        if(first_not_space != std::string::npos)
            value = value.substr(first_not_space);

        size_t last_not_space = value.find_last_not_of(" \t");
        if(last_not_space != std::string::npos)
            value = value.substr(0, last_not_space + 1);

        return value;
    }
    return "";
}



void Bartender_Summary(const std::string &macrofile, int MCID, double duration) {
    bool isConstantBins = true, isShaping = false;

    std::ofstream outfile("Bartender_summaries.txt", std::ios::app);

    outfile << "Bartender serial number (MCID): " << MCID << "\n\n";

    time_t now = time(0);
    tm* current_time = localtime(&now);
    outfile << "Date: " << asctime(current_time);
    outfile << "User Name: " << getlogin() << "\n";
    outfile << "Duration of the simulation: " << duration << " s\n\n";

    std::ifstream run_file(macrofile);
    if(!run_file)
    {
        std::cerr << "Can't open run macro file!" << std::endl;
        return;
    }

    std::string line;
    std::string brand, model;

    while(std::getline(run_file, line))
{
    line = line.substr(line.find_first_not_of(" \t"));
    
    if(line.find("Brand:") != std::string::npos)
        brand = summary_extract_value(line, "Brand:");
    else if(line.find("TypeNo:") != std::string::npos)
    {
        model = summary_extract_value(line, "TypeNo:");
        if(!brand.empty() && !model.empty())
            outfile << "SiPM: " << brand << " " << model << '\n';
    }
    else if(line.find("V =") != std::string::npos)
    {
        std::string V_value = summary_extract_value(line, "V =");
        if(!V_value.empty())
            outfile << "Voltage: " << V_value << '\n';
    }
    else if(line.find("T =") != std::string::npos)
    {
        std::string T_value = summary_extract_value(line, "T =");
        if(!T_value.empty())
            outfile << "Temperature: " << T_value << '\n';
    }
    else if(line.find("Sampling speed =") != std::string::npos)
    {
        std::string sampling_value = summary_extract_value(line, "Sampling speed =");
        if(!sampling_value.empty())
            outfile << "Sampling speed (template): " << sampling_value << '\n';
    }
    else if(line.find("R_shaper =") != std::string::npos)
    {
        std::string R_value = summary_extract_value(line, "R_shaper =");
        if(!R_value.empty())
            outfile << "R_shaper (template): " << R_value << '\n';
    }
    else if(line.find("Gain =") != std::string::npos)
    {
        std::string gain_value = summary_extract_value(line, "Gain =");
        if(!gain_value.empty())
            outfile << "Gain (template): " << gain_value << '\n';
    }
    else if(line == "Constant Bins = true")
    {
        outfile << "Constant Bins = true" << '\n';
    }
    else if(line == "Constant Bins = false")
    {
        outfile << "Constant Bins = false" << '\n';
        isConstantBins = false;
    }
    else if(!isConstantBins && line.find("BinSize sigma =") != std::string::npos)
    {
        std::string sigmaBin_value = summary_extract_value(line, "BinSize sigma =");
        if(!sigmaBin_value.empty())
            outfile << "Bin (sigma) = " << sigmaBin_value << '\n';
    }
    else if(line.find("Sampling speed_sim =") != std::string::npos)
    {
        std::string sampling_value = summary_extract_value(line, "Sampling speed_sim =");
        if(!sampling_value.empty())
            outfile << "Sampling speed: " << sampling_value << '\n';
    }
    else if(line == "Use shaping = true")
    {
        outfile << "Shaping = ON" << '\n';
        isShaping = true;
    }
    else if(line == "Use shaping = false")
    {
        outfile << "Shaping = OFF" << '\n';
    }
    else if(isShaping && line.find("Tau_shaping =") != std::string::npos)
    {
        std::string Tau_value = summary_extract_value(line, "Tau_shaping =");
        if(!Tau_value.empty())
            outfile << "Tau_shaping = " << Tau_value << '\n';
    }
    else if(line.find("Gain_sim =") != std::string::npos)
    {
        std::string gain_value = summary_extract_value(line, "Gain_sim =");
        if(!gain_value.empty())
            outfile << "Gain: " << gain_value << '\n';
    }
    else if(line.find("Noise (sigma) =") != std::string::npos)
    {
        std::string noise_value = summary_extract_value(line, "Noise (sigma) =");
        if(!noise_value.empty())
            outfile << "Noise (sigma): " << noise_value << '\n';
    }
}


    run_file.close();
    outfile << "\n########################################################\n\n";
}

#include <iostream>
#include <ctime>
#include "csv.h"
#include <iomanip>
#include <sstream>  
#include <map>
#include <unordered_map>

struct ObservationInfo
{
    
};

int main()
{


    std::vector<std::unordered_map<float, std::vector<double>>> entrances(5);
    std::vector<std::unordered_map<float, std::vector<double>>> campings(9);
    std::vector<std::unordered_map<float, std::vector<double>>> rngrStops(8);
    std::vector<std::unordered_map<float, std::vector<double>>> gates(9);
    std::vector<std::unordered_map<float, std::vector<double>>> generalGates(8);
    std::map<float, std::vector<double>> rngrbase;

    std::vector<std::vector<std::string>> lines;
    std::vector<std::string> tokens;
    std::vector<float> ids;
    char * pch;
    io::LineReader in("../sensordata.csv");

    int line = 0;
    bool first = true;
    while(char*line = in.next_line())
    {
        if (first)
        {
            first = false; 
            continue;
        }
        pch = strtok(line, ",");
        tokens.clear();
        while(pch != NULL)
        {
            tokens.push_back(pch);
            pch = strtok(NULL, ",");
        }
        lines.push_back(tokens);
        std::string id = tokens[1];
        id.erase(std::remove(id.begin(), id.end(), '-'), id.end());
        float idi = std::stof(id);
        ids.push_back(idi);
    }


    //Remove first line with lables
    assert(!lines.empty());




    for (int i = 0; i < lines.size(); ++i)
    {
        float currentId = ids[i];
        for (int j = i+1; j < lines.size(); ++j)
        {
            if (currentId == ids[j])
            {
                std::tm tm = {};
                std::tm tm2 = {};
                std::stringstream ss(lines.at(i).at(0));
                std::stringstream ss2(lines.at(j).at(0));
                ss >> std::get_time(&tm, "%Y-%M-%D %H:%M:%S");
                ss2 >> std::get_time(&tm2, "%Y-%M-%D %H:%M:%S");
                auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
                auto tp2 = std::chrono::system_clock::from_time_t(std::mktime(&tm2));
                auto trp = (tp2 - tp) * 0.0000001;

                if (lines[i][3][0] == 'e' )
                {
                    int index = int(lines[i][3][8] - 48);
                    if (entrances[index][currentId].size() == 0) {
                        std::vector<double> v;
                        v.push_back(trp.count());
                        entrances[index][currentId] = v;
                    }
                    else {
                        entrances[index][currentId].push_back(trp.count());
                    }
                }
                else if (lines[i][3][0] == 'c')
                {
                    int index = int(lines[i][3][7] - 48);
                    if (campings[index][currentId].size() == 0) {
                        std::vector<double> v;
                        v.push_back(trp.count());
                        campings[index][currentId] = v;
                    }
                    else {
                        campings[index][currentId].push_back(trp.count());
                    }
                }
                else if (lines[i][3][0] == 'r'  && lines[i][3][11] == 's')
                {
                    int index = int(lines[i][3][11] - 48);
                    if (rngrStops[index][currentId].size() == 0) {
                        std::vector<double> v;
                        v.push_back(trp.count());
                        rngrStops[index][currentId] = v;
                    }
                    else {
                        rngrStops[index][currentId].push_back(trp.count());
                    }
                }
                else if (lines[i][3][0] == 'r'  && lines[i][3][11] == 'b')
                {
                    if (rngrbase[currentId].size() == 0) {
                        std::vector<double> v;
                        v.push_back(trp.count());
                        rngrbase[currentId] = v;
                    }
                    else {
                        rngrbase[currentId].push_back(trp.count());
                    }
                }
                else if (lines[i][3][0] == 'g' && lines[i][3][1] == 'a')
                {
                    int index = int(lines[i][3][4] - 48);
                    if (gates[index][currentId].size() == 0) {
                        std::vector<double> v;
                        v.push_back(trp.count());
                        gates[index][currentId] = v;
                    }
                    else {
                        gates[index][currentId].push_back(trp.count());
                    }
                }
                else if (lines[i][3][0] == 'g' && lines[i][3][1] == 'e')
                {
                    int index = int(lines[i][3][12] - 48);
                    if (generalGates[index][currentId].size() == 0) {
                        std::vector<double> v;
                        v.push_back(trp.count());
                        generalGates[index][currentId] = v;
                    }
                    else {
                        generalGates[index][currentId].push_back(trp.count());
                    }
                }
                
                //Remove first line with lables
                //assert(!lines.empty());
                //lines.erase(lines.begin());*/
                continue;
            }
        }
    }

    std::cout << "done";
    std::cin.get();
    return 0;
}

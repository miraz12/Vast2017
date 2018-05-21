#include <iostream>
#include <ctime>
#include "csv.h"
#include <iomanip>
#include <sstream>  
#include <map>
#include <unordered_map>
#include "astar.h"



int main()
{
    mapWidth = 200;
    mapHeight = 200;
    int size = mapWidth * mapHeight;

    std::vector<std::unordered_map<float, std::vector<double>>> mentrances(5);
    std::vector<std::unordered_map<float, std::vector<double>>> mcampings(9);
    std::vector<std::unordered_map<float, std::vector<double>>> mrngrStops(8);
    std::vector<std::unordered_map<float, std::vector<double>>> mgates(9);
    std::vector<std::unordered_map<float, std::vector<double>>> mgeneralGates(8);
    std::map<float, std::vector<double>> mrngrbase;

    std::vector<std::vector<std::string>> lines;
    std::vector<std::string> tokens;
    std::vector<std::string> ids;
    char * pch;
    io::LineReader in("../sensordata.csv");

    clock_t start, end;
    double cpu_time_used;

    start = clock();
    

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
        //id.erase(std::remove(id.begin(), id.end(), '-'), id.end());
        //double idi = std::stof(id);
        ids.push_back(id);
    }


    //Remove first line with lables
    assert(!lines.empty());


    std::cout << "Loading bmp" << std::endl;
    readBMP("../map24.bmp");
    std::cout << "Building map" << std::endl;
    buildMap(pMap);

    int inter = 0;
    int i = 0;
    for (int i = 0; i < lines.size(); ++i)
    {
        std::string currentId = ids[i];
        for (int j = i+1; j < lines.size(); ++j)
        {
            if (currentId == ids[j])
            {
                int x1{ -1 }, y1{ -1 }, x2{ -1 }, y2{ -1 };
                if (lines[i][3][0] == 'e' )
                {
                    int index = int(lines[i][3][8] - 48);
                    x1 = entrances[index].x;
                    y1 = entrances[index].y;
                    
                }
                else if (lines[i][3][0] == 'c')
                {
                    int index = int(lines[i][3][7] - 48);
                    x1 = campings[index].x;
                    y1 = campings[index].y;

                }
                else if (lines[i][3][0] == 'r'  && lines[i][3][7] == 's')
                {
                    int index = int(lines[i][3][11] - 48);
                    x1 = rngrStops[index].x;
                    y1 = rngrStops[index].y;

                }
                else if (lines[i][3][0] == 'r'  && lines[i][3][7] == 'b')
                {
                    x1 = rngrbase.x;
                    y1 = rngrbase.y;

                }
                else if (lines[i][3][0] == 'g' && lines[i][3][1] == 'a')
                {
                    int index = int(lines[i][3][4] - 48);
                    x1 = gates[index].x;
                    y1 = gates[index].y;

                }
                else if (lines[i][3][0] == 'g' && lines[i][3][1] == 'e')
                {
                    int index = int(lines[i][3][12] - 48);
                    x1 = generalGates[index].x;
                    y1 = generalGates[index].y;
                }

                if (lines[j][3][0] == 'e')
                {
                    int index = int(lines[j][3][8] - 48);
                    x2 = entrances[index].x;
                    y2 = entrances[index].y;

                }
                else if (lines[j][3][0] == 'c')
                {
                    int index = int(lines[j][3][7] - 48);
                    x2 = campings[index].x;
                    y2 = campings[index].y;

                }
                else if (lines[j][3][0] == 'r'  && lines[j][3][7] == 's')
                {
                    int index = int(lines[j][3][11] - 48);
                    x2 = rngrStops[index].x;
                    y2 = rngrStops[index].y;

                }
                else if (lines[j][3][0] == 'r'  && lines[j][3][7] == 'b')
                {
                    x2 = rngrbase.x;
                    y2 = rngrbase.y;


                }
                else if (lines[j][3][0] == 'g' && lines[j][3][1] == 'a')
                {
                    int index = int(lines[j][3][4] - 48);
                    x2 = gates[index].x;
                    y2 = gates[index].y;
                }
                else if (lines[j][3][0] == 'g' && lines[j][3][1] == 'e')
                {
                    int index = int(lines[j][3][12] - 48);
                    x2 = generalGates[index].x;
                    y2 = generalGates[index].y;
                }

                std::tm tm = {};
                std::tm tm2 = {};
                std::stringstream ss(lines.at(i).at(0));
                std::stringstream ss2(lines.at(j).at(0));
                ss >> std::get_time(&tm, "%Y-%D-%M %H:%M:%S");
                ss2 >> std::get_time(&tm2, "%Y-%D-%M %H:%M:%S");
                auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
                auto tp2 = std::chrono::system_clock::from_time_t(std::mktime(&tm2));
                auto trp = (tp2 - tp) * 0.0000001 / 60 / 60;

                int type = lines[j][2][0]-48;

                if (lines[i][2][1] == NULL)
                {
                    FindDistance(x1, y1, x2, y2, tm.tm_mon, 0);
                    inter++;
                }



                //Remove first line with lables
                //assert(!lines.empty());
                //lines.erase(lines.begin());*/
                break;
            }
        }
    }


    printf("%i \n", numSpeedViol);
    printf("%i \n", inter);
    std::cout << "Exporting heatmap" << std::endl;
    exportHeatmap();

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    std::cout << "done: " << cpu_time_used;
    std::cin.get();
    return 0;
}

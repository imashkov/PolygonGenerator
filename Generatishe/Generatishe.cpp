#include "Logger.h"
#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
#include <limits.h>
#include <regex>
#include <unordered_map>
#include <vector>
#include <math.h>

namespace generator
{
    // generating values and points
    class Randomizer {
    public:
        std::mt19937 engine;
        Randomizer()
        {
            engine.seed(std::time(nullptr));
        }
        int getRandomNumber()
        {
            return engine();
        }
        void getRandomPoint()
        {

        }
        int getRandomCoor(int max, int min)
        {
            return engine() % (max - min) + min;
        }
        void getRandomPointInArea(int maxX, int maxY, std::pair<int, int>& point)
        {
            point.first = engine() % maxX;
            point.second = engine() % maxY;
        }
    };
    // storage and filling of input data and main params 
    class Configuration {
    public:
        std::unordered_map<std::string, int> configuration;
        Configuration()
        {
            configuration["area_size"] = INT32_MAX;
            configuration["polygons_count"] = 10;
            configuration["max_points"] = 5;
            configuration["min_points"] = 5;
            configuration["max_width"] = 10000;
            configuration["min_width"] = 100;
            configuration["max_height"] = 10000;
            configuration["min_height"] = 100;
            configuration["max_distance"] = 100000;
            configuration["min_distance"] = 10;
            configuration["gaps_count"] = 20;
        }
        Configuration(std::unordered_map<std::string, int> argsMap) : Configuration()
        {
            for (auto iter = argsMap.begin(); iter != argsMap.end(); iter++)
                this->configuration[iter->first] = argsMap[iter->first];
        }
        void CreateRandomConfig()
        {

        }
        void PrintConfiguration()
        {
            for (auto iter = configuration.begin(); iter != configuration.end(); iter++)
                std::cout << iter->first << " = " << iter->second << '\n';
        }
    };

    class Builder
    {
    private:
        Randomizer randomizer;
        std::vector<std::vector<std::pair<int, int>>> test;
    public:
        Configuration config;
        Builder() {}
        Builder(std::unordered_map<std::string, int> config) { this->config = Configuration(config); }
        std::vector<std::vector<std::pair<int, int>>> GetResult() { return test; }
        void StartCreating()
        {

            ZoningAlgorightm();
        }
        bool CheckPolygon(std::vector<std::pair<int, int>>& polygon)
        {
            for (std::pair<int, int> point : polygon)
            {
                if (point.first < 0 || point.second < 0)
                {
                    LOGGER::Logger::GetInstance()->WriteLog(LOGGER::LogLevel::Warning,
                        "Here is negative coor.\nCreating new test...");
                    return 1;
                }
            }
            return 0;
        }
        bool CheckTest()
        {
            if (test.size() != config.configuration["polygons_count"])
            {
                LOGGER::Logger::GetInstance()->WriteLog(LOGGER::LogLevel::Warning, "Not all polygons are built./nCreating new test...");
                return 1;
            }
            for (int i = 0; i < test.size(); i++)
                if (CheckPolygon(test[i]) == 1)
                    return 1;
            return 0;
        }
        void MakeGap(std::vector<std::pair<int, int>>& polygon)
        {
            std::pair<int, int> tmpPoint, firstPoint;
            int maxH = polygon[1].second - polygon[0].second - 2;
            int maxW = polygon[2].first - polygon[1].first - 2;

            this->randomizer.getRandomPointInArea(maxW, maxH, tmpPoint);
            polygon.push_back(tmpPoint);
            firstPoint = tmpPoint;
            // second point, only y changed
            tmpPoint.second += this->randomizer.getRandomCoor(maxH - tmpPoint.second, 2);
            polygon.push_back(tmpPoint);
            // third point, only x changed
            tmpPoint.first += this->randomizer.getRandomCoor(maxW - tmpPoint.first, 2);
            polygon.push_back(tmpPoint);
            tmpPoint.second = firstPoint.second;
            polygon.push_back(tmpPoint);
            polygon.push_back(firstPoint);
            polygon.push_back(polygon[0]);
        }
        void FullRandomAlgorightm()
        {

        }
        void VariableAlgorightm()
        {

        }
        void ZoningAlgorightm()
        {
            // net proverki na max_distance. Nado sizecoef podbirat iz ornosheniya max_size/max_distant. No eto poka togo ne stoit.
            bool flag = 0; int currentPolygonsCount = config.configuration["polygons_count"];
            int currentGapsCount = config.configuration["gaps_count"];
            int minDistance = config.configuration["min_distance"];
            std::pair<int, int> tmpPoint;
            std::vector<std::pair<int, int>> tmpPolygon;
            // int maxS = config.configuration["max_width"] * config.configuration["max_height"];
            int sizeCoef = 4;
            int rowSize = ceil(sqrt(config.configuration["polygons_count"]));
            int areaX = ceil(sqrt(sizeCoef)) * config.configuration["max_width"];
            int areaY = ceil(sqrt(sizeCoef)) * config.configuration["max_height"];
            int availableAreaX = areaX - config.configuration["min_width"];
            int availableAreaY = areaY - config.configuration["min_height"];

            for (int i = 0; i < rowSize; i++)
            {
                for (int j = 0; j < rowSize; j++)
                {
                    if (currentPolygonsCount == 0)
                        return;
                    tmpPolygon.clear();
                    tmpPoint.first = 0;
                    tmpPoint.second = 0;
                    this->randomizer.getRandomPointInArea(availableAreaX, availableAreaY, tmpPoint);
                    tmpPolygon.push_back(tmpPoint);
                    // second point, only y changed
                   
                    int maxH = min(areaY - tmpPoint.second, config.configuration["max_height"]);
                    tmpPoint.second += this->randomizer.getRandomCoor(maxH, config.configuration["min_height"]);
                    tmpPolygon.push_back(tmpPoint);
                    // third point, only x changed
                    int maxW = min(areaX - tmpPoint.first, config.configuration["max_width"]);
                    tmpPoint.first += this->randomizer.getRandomCoor(maxW, config.configuration["min_width"]);
                    tmpPolygon.push_back(tmpPoint);
                    tmpPoint.second = tmpPolygon[0].second;
                    tmpPolygon.push_back(tmpPoint);
                    tmpPoint.first = tmpPolygon[0].first;
                    tmpPolygon.push_back(tmpPoint);

                    for (std::pair<int, int>& point : tmpPolygon)
                    {
                        point.first += (areaX + minDistance) * i;
                        point.second += (areaY + minDistance) * j;
                    }
                    if (CheckPolygon(tmpPolygon) == 1)
                    {
                        // nado pereschitat polygon
                    }
                    if (currentGapsCount > 0)
                    {
                        MakeGap(tmpPolygon);
                        currentGapsCount--;
                    }
                    this->test.push_back(tmpPolygon);
                    currentPolygonsCount--;
                }
            }
            if (CheckTest() == 1)
            {
                test.clear();
                ZoningAlgorightm();
            }
        }
        void ConstantAlgorithm()
        {

        }
    };

    class Generator {
    private:
        std::string filePath;
    public:
        Generator() { filePath = ""; }
        ~Generator() {}

        void initCreating(int argsCount, char* args[])
        {
            std::unordered_map<std::string, int> argsMap;
            int error = readInput(argsCount, args, argsMap);
            Builder builder = Builder(argsMap);
            builder.StartCreating();

            writeInFile(this->filePath, builder.GetResult());
        }

        int readInput(int argsCount, char* args[], std::unordered_map<std::string, int>& argsMap)
        {
            if (argsCount > 0)
            {
                // maybe try regex
                std::string tmp;
                for (int i = 0; i < argsCount; i++)
                {
                    tmp = args[i];
                    if (tmp.find(".txt") != std::string::npos)
                        this->filePath = tmp;
                    if (tmp.find("-help") != std::string::npos)
                        ConsoleHelp();
                    if (tmp.find("=") != std::string::npos)
                    {
                        int value = std::atoi(tmp.substr(tmp.find("=") + 1).c_str());
                        if (tmp.find("area_size") != std::string::npos)
                            argsMap["area_size"] = value;
                        if (tmp.find("polygons_count") != std::string::npos)
                            argsMap["polygons_count"] = value;
                        if (tmp.find("max_points") != std::string::npos)
                            argsMap["max_points"] = value;
                        if (tmp.find("min_points") != std::string::npos)
                            argsMap["min_points"] = value;
                        if (tmp.find("max_width") != std::string::npos)
                            argsMap["max_width"] = value;
                        if (tmp.find("min_width") != std::string::npos)
                            argsMap["min_width"] = value;
                        if (tmp.find("max_height") != std::string::npos)
                            argsMap["max_height"] = value;
                        if (tmp.find("min_height") != std::string::npos)
                            argsMap["min_height"] = value;
                        if (tmp.find("max_distance") != std::string::npos)
                            argsMap["max_distance"] = value;
                        if (tmp.find("min_distance") != std::string::npos)
                            argsMap["min_distance"] = value;
                        if (tmp.find("gaps_count") != std::string::npos)
                            argsMap["gaps_count"] = value;
                    }
                }
                if (filePath == "")
                {
                    LOGGER::Logger::GetInstance()->WriteLog(LOGGER::LogLevel::Warning,
                       "Missing parameter file name. File will be created by generator.");
                    std::fstream file("new_test.txt");
                    file.close();
                    this->filePath = "new_test.txt";
                }
                if (validateInputParams(argsMap))
                    return 1;
            }
        }

        bool validateInputParams(std::unordered_map<std::string, int>& argsMap)
        {
            for (auto iter = argsMap.begin(); iter != argsMap.end(); iter++)
            {
                if (iter->second < 0)
                {
                    LOGGER::Logger::GetInstance()->WriteLog(LOGGER::LogLevel::Error, iter->first + " parameter can not be negative");
                    return 1;
                }
                if (iter->first == "polygons_count" && iter->second > 100000)
                {
                    LOGGER::Logger::GetInstance()->WriteLog(LOGGER::LogLevel::Error, "Count of polygons is out of range.");
                    return 1;
                }
                if (iter->first == "gaps_count" && iter->second > argsMap["polygons_count"])
                {
                    LOGGER::Logger::GetInstance()->WriteLog(LOGGER::LogLevel::Error, "Count of polygons with gaps exceeds count of all polygons");
                    return 1;
                }
                if (iter->first == "min_height" && iter->second > argsMap["max_height"])
                {
                    LOGGER::Logger::GetInstance()->WriteLog(LOGGER::LogLevel::Error, "Min height param > max height.");
                    return 1;
                }
                if (iter->first == "min_width" && iter->second > argsMap["max_width"])
                {
                    LOGGER::Logger::GetInstance()->WriteLog(LOGGER::LogLevel::Error, "Min width param > max width.");
                    return 1;
                }
                if (iter->first == "min_distance" && iter->second > argsMap["max_distance"])
                {
                    LOGGER::Logger::GetInstance()->WriteLog(LOGGER::LogLevel::Error, "Min distance param > max distance.");
                    return 1;
                }
                if (iter->first == "min_points" && iter->second > argsMap["max_points"])
                {
                    LOGGER::Logger::GetInstance()->WriteLog(LOGGER::LogLevel::Error, "Min points count > max points count.");
                    return 1;
                }
            }
            return 0;
        }

        void writeInFile(std::string& path, std::vector<std::vector<std::pair<int, int>>> finishedTest)
        {
            std::ofstream testFile(path/*, std::ios::app*/);
            testFile << "Count  " << finishedTest.size() << '\n';
            for (std::vector<std::pair<int, int>> polygon : finishedTest)
            {
                testFile << "\nBoundary    " << polygon.size() << " ";
                for (std::pair<int, int> point : polygon)
                    testFile << point.first << ", " << point.second << "; ";
            }
            testFile.close();
        }
        void ConsoleHelp()
        {
            std::cout << "/nUsage: ./Generator/Debug/Generator.exe [-parameters=values]. All parameters have default value.Parameter list:\n";
            std::cout << "area_size\n";
            std::cout << "polygons_count\n";
            std::cout << "max_points\n";
            std::cout << "min_points\n";
            std::cout << "max_width\n";
            std::cout << "min_width\n";
            std::cout << "min_height\n";
            std::cout << "max_distance\n";
            std::cout << "min_distance\n";
            std::cout << "gaps_count\n";
            std::cout << "*.txt\n\n";
            std::cout << "Example of using: ./Generator.exe -polygons_count=200 -min_width=500 ../test200n.1.txt\n";
            std::cout << "Will be created test file with 200 polygons with the minimal width 500, result will be written in test200n.1.txt file\n\n";
        }
    };
}
int main(int argc, char* argv[])
{
    generator::Generator generator = generator::Generator();
    generator.initCreating(argc, argv);
    return 0;
}
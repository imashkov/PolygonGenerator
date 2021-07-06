#pragma once
#include <iostream>
#include <windows.h>
#include <sstream>
#include <cassert>
#include <chrono>
#include <fstream>

namespace LOGGER
{
    enum class LogLevel
    {
        Info,
        Debug,
        Warning,
        Error
    };

    class Logger {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> time;
        Logger();
        static Logger* logger_;
    public:
        void operator=(const Logger l) = delete;
        static Logger* GetInstance();
        void WriteLog(LogLevel logLevel, std::string message);
        void ShowLog(std::string filePath = "LogFile.txt");
        void ResetTimer() { time = std::chrono::high_resolution_clock::now(); }
        void GetTime(std::string message = "", bool writeToConsole = 0, int unit = 0);
        void ClearLogFile();
    };
    Logger* Logger::logger_ = nullptr;
    LOGGER::Logger* LOGGER::Logger::GetInstance()
    {
        if (logger_ == nullptr)
            logger_ = new Logger();
        return logger_;
    }
    LOGGER::Logger::Logger()
    {
        time = std::chrono::high_resolution_clock::now();
        std::ofstream logFile("Logfile.txt", std::ios::app);
        for (int i = 0; i < 60; i++)
            logFile << "_";
        logFile << "\n\n\nNew launch\n";
        logFile.close();
    }
    void LOGGER::Logger::WriteLog(LogLevel logLevel, std::string message)
    {
        std::ofstream logFile("Logfile.txt", std::ios::app);
        if (logLevel == LogLevel::Info)
        {
            logFile << message << "\n";
        }
        if (logLevel == LogLevel::Debug)
        {
            logFile << message << "\n";
            std::cout << message << "\n";
        }
        if (logLevel == LogLevel::Warning)
        {
            logFile << "Warning: " << message << "\n";
            std::cout << "Warning: " << message << "\n";
        }
        if (logLevel == LogLevel::Error)
        {
            logFile << "Error: " << message << "\n";
            logFile.close();
            std::cerr << "Error: " << message << "\n";
            assert(0);
        }
        logFile.close();
    }

    void LOGGER::Logger::ShowLog(std::string fileName)
    {
        std::cout << "Log file output:\n";
        std::ifstream logFile("Logfile.txt", std::ios::app);
        if (logFile.is_open())
        {
            std::string tmp;
            while (!logFile.eof())
            {
                std::getline(logFile, tmp);
                std::cout << tmp << std::endl;
            }
        }
        else
            std::cout << "Log file doesn't exist\n";
        logFile.close();
    }

    void LOGGER::Logger::GetTime(std::string message, bool writeToConsole, int unit)
    {
        auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - time).count();
        ResetTimer();
        if (unit == 1)
            timeDiff = timeDiff / 1000;
        std::string tmp = message + "\n" + std::to_string(timeDiff);
        if (unit == 1)
            tmp += " s passed.\n";
        else
            tmp += " ms passed.\n";
        LogLevel logLevel = LogLevel::Info;
        if (writeToConsole)
            logLevel = LogLevel::Debug;
        WriteLog(logLevel, tmp);
    }
    void LOGGER::Logger::ClearLogFile()
    {
        std::ofstream logFile("Logfile.txt");
        logFile.close();
    }
}

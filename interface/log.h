#pragma once

#include <string>
#include <exception>
#include <fstream>
#include <vector>
#include <iostream>
#include <filesystem>

#define EXCEPT_TEXT(x) std::filesystem::path(__FILE__).filename().string() + "[" + std::to_string(__LINE__) + "] \n\tREASON: " + x


namespace Interface
{
    // Abstract class used to interface with some text medium for text output
    class OutputWrapper
    {
    public:
        virtual void write(const std::string& msg) = 0;
    };

    // Write To Cout Wrapper
    class stdOutput : public OutputWrapper
    {
    public:
        void write(const std::string& msg) override { std::cout << msg; }
    };

    // Save To File Wrapper
    class fileOutput : public OutputWrapper
    {
        std::ofstream file;
    public:
        fileOutput(const std::string& path) { file.open(path); };
        ~fileOutput() { file.close(); }
        void write(const std::string& msg) override { if (file.is_open()) file.write(msg.data(), msg.size()); }
    };

    enum class Level
    {
        EXCEPTION,      // Only Print Fatal Exceptions
        SILENT,         // No Output
        ERROR,          // Print Potentially Recoverable Errors
        LOG,            // Print Standard Ouput
        VERBOSE         // Print Verbose Output
    };

    class Log
    {
        mutable Level logLevel = Level::LOG;
        std::vector<OutputWrapper*> probes{};

        mutable std::vector<std::string> history;
    public:
        ~Log();
        void setLevel(const Level&) const;
        void log(const Level& severity, const std::string& msg) const;
        void saveHistory(const std::string& path) const;
        void attachProbe(OutputWrapper* newProbe);
    };
}

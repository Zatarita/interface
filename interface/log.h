#pragma once

#include <string>
#include <exception>
#include <fstream>
#include <vector>
#include <iostream>

namespace Interface
{
    enum class Flags
    {
        SILENT,         // No Output
        EXCEPTION,      // Only Print Fatal Exceptions
        ERROR,          // Print Potentially Recoverable Errors
        LOG,            // Print Standard Ouput
        VERBOSE         // Print Verbose Output
    };

    class Log
    {
        Flags logLevel = Flags::LOG;
        std::ostream* streamBuffer = &std::cout;

        mutable std::vector<std::string> history;
    public:
        void setLevel(const Flags&);
        void log(const Flags& severity, const std::string& msg) const;
        void saveHistory(const std::string& path) const;
        void redirectBuffer(std::ostream* newBuffer);
    };

    static Log globalLog;
    static inline void emit(const Flags& severity, const std::string& msg) {  globalLog.log(severity, msg); }
}

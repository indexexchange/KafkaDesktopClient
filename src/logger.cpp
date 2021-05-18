#include "logger.h"
#include <string>
#include <iostream>
#include <fstream>


namespace logger {

    // default to level logger_info
    Level Log::ReportingLevel = Level::kInfo;

    // default to writing to std::cout
    std::ostream* Log::stream = &std::cout;

    std::ostringstream& Log::Get(Level level)
    {
        switch (level)
        {
        case Level::kError:  os << "[E]"; break;
        case Level::kInfo:   os << "[I]"; break;
        case Level::kDebug:  os << "[D]"; break;
        case Level::kTrace:  os << "[T]"; break;
        }
        return os;
    }

    Log::~Log()
    {
        os << std::endl;
        *stream << os.str();
        stream->flush();
    }

    void WriteToFile(const std::string& filename) {
        static std::ofstream outfile;
        outfile.open(filename, std::ofstream::out | std::ofstream::app);
        Log::stream = &outfile;
    }

    void SetLevelInfo()
    {
        Log::ReportingLevel = Level::kInfo;
    }

    void SetLevelDebug()
    {
        Log::ReportingLevel = Level::kDebug;
    }

    void SetLevelTrace()
    {
        Log::ReportingLevel = Level::kTrace;
    }

}


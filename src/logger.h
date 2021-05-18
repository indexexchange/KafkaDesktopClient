#ifndef LOGGER_H_
#define LOGGER_H_

#include <thread>
#include <sstream>

#define logger_err  LOG(logger::Level::kError, "")
#define logger_info LOG(logger::Level::kInfo,  "")
#define logger_debug LOG(logger::Level::kDebug,"")
#define logger_trace LOG(logger::Level::kTrace,"")

#define trace_entry LOG(logger::Level::kTrace,"->")
#define trace_exit LOG(logger::Level::kTrace,"<-")

#define LOG(level, tag) \
if (level > logger::Log::ReportingLevel) ; \
else logger::Log().Get(level)<<"[" << std::hex << std::this_thread::get_id() << "][" << __FUNCTION__ << ":" << std::dec << __LINE__ << "]: "<<tag



namespace logger {

    // Convenience functions
    void WriteToFile(const std::string&);   // redirect logs to a file rather than std::cout
    void SetLevelInfo();                    // set logging level
    void SetLevelDebug();
    void SetLevelTrace();


    // Loggign levels allowed
    enum class Level {
        kError, kWarning, kInfo, kDebug, kTrace
    };

    // Accumulate and write user logs
    class Log
    {
    public:
        Log() { }
        Log(const Log&) = delete;
        Log& operator =(const Log&) = delete;

        std::ostringstream& Get(Level level = Level::kInfo);

        virtual ~Log();

    public:
        static Level ReportingLevel;
        static std::ostream* stream;
        
    protected:
        std::ostringstream os;
    private:
        Level messageLevel = Level::kInfo;
    
    }; // Log

} // logger

#endif // LOGGER_H_
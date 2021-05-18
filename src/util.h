#ifndef UTIL_H_
#define UTIL_H_

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <sstream>
#include <ctime>


#include <nlohmann/json.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "logger.h"

using json = nlohmann::json;

namespace util {

    inline std::shared_ptr<std::string> ReadFile(std::string path) {
        try {
            std::ifstream instream(path);
            std::stringstream buffer;
            buffer << instream.rdbuf();

            auto contents = std::make_shared<std::string>(buffer.str());
            return contents;

        }
        catch (...) {
            logger_err << "Could not read file: " << path;
            return std::shared_ptr<std::string>();
        }
    }

    inline void WriteFile(std::string path, std::string data) {
        std::ofstream outstream;

        outstream.open(path);

        if (!outstream) {
            logger_err << "could not open file for writing";
            return;
        }

        outstream << data;
        outstream.close();
    }

    inline bool is_substring(std::string first, std::string second) {
        if (first.find(second) != std::string::npos) {
            return true;
        }
        return false;
    }

    inline void clear_file_contents(std::string filename) {
        // Delete contents of scratch
        std::ofstream file;
        file.open(filename, std::ofstream::out | std::ofstream::trunc);
        file.close();
    }

    inline void for_each_line_in_file(std::string filename, std::function<void(std::string)> func) {
        std::ifstream file(filename);
        try {
            std::string line;
            while (std::getline(file, line)) {
                func(line);
            }
        }
        catch(...){
        }
        file.close();
    }

    inline std::string encode_url(std::string url) {
        if (url.empty()) {
            return "";
        }

        std::string encoded;

        CURL* curl = curl_easy_init();
        if (curl) {
            char* output = curl_easy_escape(curl, url.c_str(), url.size());
            if (output) {
                encoded = output;
                curl_free(output);
            }
        }
        return encoded;
    }

    inline std::string GET(std::string url) {
        try {
            // RAII cleanup
            curlpp::Cleanup myCleanup;
            std::ostringstream data;
            data << curlpp::options::Url(url);
            return data.str();
        }
        catch (curlpp::RuntimeError& e)
        {
            //logger_debug << e.what();
            std::cout << e.what() << std::endl;
        }

        catch (curlpp::LogicError& e)
        {
            //logger_debug << e.what();
            std::cout << e.what() << std::endl;
        }
        catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        return "";
    }

    inline long date_to_epoch(std::string date) {    //must be in format 2021-02-02
        auto year = date.substr(0, 4); // 2021-02-02
        auto month = date.substr(5, 2);
        auto day = date.substr(8, 2);

        struct tm t;
        time_t epoch;
        t.tm_year = std::stoi(year) - 1900;
        t.tm_mon = std::stoi(month) - 1;
        t.tm_mday = std::stoi(day);
        t.tm_hour = 0;
        t.tm_min = 0;
        t.tm_sec = 0;
        t.tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
        epoch = mktime(&t);

        return (long)epoch;
    }

    inline std::string epoch_to_date(std::string timestr) {
        
        //string timestr = "1612242000000000";
        long long rawtime = stoll(timestr);

        // Convert from micro to seconds
        time_t epochtime = rawtime / 1000000LL;

        struct tm date;
        date = *std::localtime(&epochtime);

        // Uses a fixed-length buffer for `strtftime`
        char buffer[256];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &date);

        return std::string(buffer);
    }

    inline bool is_valid_json(std::shared_ptr<std::string> str_ptr) {
        if (json::accept(*str_ptr)) {
            return true;
        }
        return false;
    }

    inline std::shared_ptr<std::string> try_json_array(std::shared_ptr<std::string> str_ptr) {
        // Try to convert to a json array. If conversion fails, return original.

        // Check if json is already valid
        if (is_valid_json(str_ptr)) {
            return str_ptr;
        }

        // Try to convert to json array
        std::stringstream ss;
        ss << "[\"";
        for (auto& c : *str_ptr) {
            if (c == '\t') {
                ss << "\",\"";
            }
            else {
                ss << c;
            }
        }
        ss << "\"]";
        auto arr_ptr = std::make_shared<std::string>(ss.str());

        if (is_valid_json(arr_ptr)) {
            return arr_ptr;
        }
        
        // could not convert in any way, just return original string then
        return str_ptr;
    }
}

#endif // UTIL_H_
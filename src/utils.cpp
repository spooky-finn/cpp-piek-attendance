#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "utils.hpp"

namespace utils {
    std::string getRootDir(std::string _path) { 
        static std::string path;
        if (_path.length() == 0) return path;
        path = _path; 
        return path; 
    } 

    std::string getZkDatabasePath(std::string _path){
        static std::string path;
        if (_path.length() == 0) return path;
        path = _path; 
        return path; 
    }

    vector<string> split (string s, string delimiter) {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        string token;
        vector<string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
            token = s.substr (pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back (token);
        }

        res.push_back (s.substr (pos_start));
        return res;
    }

    string removeQuotes(string &str){
        str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
        return str;
    }

    long getTimedelta(const time_t &t1, const time_t &t2){
        return abs(t1-t2);
    }

    time_t addTimeToTime(time_t rawtime, int additional_days){
        return rawtime + 60*60*24*additional_days;
    };

    time_t str2unixtime(const std::string timestamp, const std::string str_format){
        if (timestamp.length() == 0) 
            return 0;

        const char *time_details = timestamp.c_str();
        const char *format = str_format.c_str();
        struct tm tm;
        strptime(time_details, format, &tm );

        tm.tm_isdst = 0;
        return mktime(&tm);

    };

    std::string unixtime2str(const time_t &rawtime){
        if (rawtime == 0 ) return "";
        tm * curr_tm;
        char timestamp[100];
        
        curr_tm = localtime(&rawtime);
        
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", curr_tm);
        return string(timestamp);
    }

}
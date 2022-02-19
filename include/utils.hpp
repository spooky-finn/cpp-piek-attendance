#pragma once
#include <iostream>
#include <string>
#include <vector>

#define CONFIG_PATH "/etc/cpp_piek_attendance/"

using namespace std;

namespace utils {
    std::string getRootDir(std::string _path = "");
    std::string getZkDatabasePath(std::string _path = "");

    vector<string> split (string s, string delimiter);

    string removeQuotes(string &str);

    // return the time in seconds between two timestamps ( in format "%Y-%m-%d %H:%M:%S" by default)
    long getTimedelta(const time_t &t1, const time_t &t2);

    time_t addTimeToTime(time_t rawtime, int additional_days);
    time_t str2unixtime(const std::string timestapm, const std::string str_format);
    std::string unixtime2str(const time_t &timestamp);
}   
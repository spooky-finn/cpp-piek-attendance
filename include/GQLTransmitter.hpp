#pragma once
#include <string>

#include "endpoint.h"
#include "types.hpp"
#include "json.hpp"

class GQLTransmitter{
    std::string hasura_secret_header = HASURA_SECRET_HEADER;
    std::string graphql_endpoint = GRAPHQL_ENDPOINT;
    
public: 
    GQLTransmitter(){};
    nlohmann::json getEmployees();
    bool insertEmployee(const vector<Employee> &users);
    bool deleteEmployee(const vector<Employee> &users);
    bool insertIntervals(const vector<Employee> &users);

    time_t getlatestEventTimestamp();
private:
    std::string prepareGQLRequest(const string request);
    std::string GQLhttpLink(const std::string &payload);
    
    // size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, std::string *s);
};
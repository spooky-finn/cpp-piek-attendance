#include <string>
#include <vector>
#include <iostream>

#include "curl/curl.h"

#include "types.hpp"
#include "GQLTransmitter.hpp"
#include "utils.hpp"
#include "json.hpp"


nlohmann::json GQLTransmitter::getEmployees(){
    vector<Employee> existEmployees;

    const std::string query = R"(
        query MyQuery {
        attendance_users {
            card
            firstname
            lastname
        }
    }
    )";

    nlohmann::json j{};
    j["query"] = prepareGQLRequest(query);

    const std::string reqBody = j.dump();
    const nlohmann::json res = nlohmann::json::parse( this->GQLhttpLink(reqBody) );
    return res["data"]["attendance_users"];
}

bool GQLTransmitter::insertEmployee(const vector<Employee> &users){
    vector<nlohmann::json> users_pool;
    users_pool.reserve(users.size());

    for (auto user : users){
        nlohmann::json a{};
        a["card"] = user.card;
        a["firstname"] = user.first_name;
        a["lastname"] = user.last_name;
        users_pool.push_back(a);
    }
    
    std::string query = R"(
        mutation ($objects: [attendance_users_insert_input!]!) {
            insert_attendance_users(objects: $objects){   
            returning {
            card
            firstname
            lastname
            }
        }
        }
    )";

    nlohmann::json j{};
    j["query"] = this->prepareGQLRequest(query);
    j["variables"]["objects"] = users_pool;

    std::string reqBody = j.dump();
    const nlohmann::json res = nlohmann::json::parse( this->GQLhttpLink(reqBody) );
    cout << "---  Inserted users ---" << endl;
    for (auto u: res["data"]["insert_attendance_users"]["returning"]){            

        std::cout << std::left
        << u["firstname"].get<std::string>() << " " 
        << u["lastname"].get<std::string>() << " "
        << u["card"].get<std::string>() 
        << endl;
    }
    return true;
}

bool GQLTransmitter::deleteEmployee(const vector<Employee> &users){
    vector<std::string> users_pool;
    users_pool.reserve(users.size());

    for (auto user : users){
        users_pool.push_back(user.card);
    }

    std::string query = R"(
        mutation ($card_list: [String]!){
        delete_attendance_users(where: {card: {_in: $card_list }}) {
            returning {
            card
            firstname
            lastname
            }
        }
        }
    )";

    nlohmann::json j{};
    j["query"] = prepareGQLRequest(query);
    j["variables"]["card_list"] = users_pool;

    std::string reqBody = j.dump();
    const nlohmann::json res = nlohmann::json::parse( this->GQLhttpLink(reqBody) );

    cout << "---  Removed users ---" << endl;
    for (auto u: res["data"]["delete_attendance_users"]["returning"]){            
        std::cout
        << u["firstname"].get<std::string>() << " " 
        << u["lastname"].get<std::string>() << " "
        << u["card"].get<std::string>() 
        << endl;
    }
    return true;
}

bool GQLTransmitter::insertIntervals(const vector<Employee> &users){
    vector<nlohmann::json> intervals_pool;

    for (auto user : users){
        for (auto interval: user.intervals){
            nlohmann::json a{};
            std::string ext = "NULL";
            
            a["card"] = interval.card;
            a["ent"] = utils::unixtime2str(interval.ent);
            if (interval.ext) a["ext"] = utils::unixtime2str(interval.ext);
            // a["dur"] = interval.dur;
            intervals_pool.push_back(a);
        }
    }

    std::string query = R"(
        mutation pushIntervals($objects: [attendance_intervals_insert_input!]!) {
        insert_attendance_intervals(objects: $objects){   
            returning {
            id
            }
        }
    }
    )";

    nlohmann::json j{};
    j["query"] = prepareGQLRequest(query);
    j["variables"]["objects"] = intervals_pool;

    std::string reqBody = j.dump();
    const nlohmann::json res = nlohmann::json::parse( this->GQLhttpLink(reqBody) );
    cout << "---  Injected intervals ---" << endl;
    cout << res << endl;
    return true;
}

time_t GQLTransmitter::getlatestEventTimestamp()
{
    static std::string sinceDate;

    if (!sinceDate.empty()) 
        return utils::str2unixtime(sinceDate, "%Y-%m-%dT%H:%M:%S");
    else
        cout << "Request at hasura.piek.ru in process." << endl;

    std::string query = R"(query MyQuery {
        attendance_intervals_aggregate {
            aggregate {
                max {
                    ext
                }
            }
        }
        })";

    nlohmann::json req{};
    req["query"] = this->prepareGQLRequest(query);

    std::string reqBody = req.dump();
    std::string res = this->GQLhttpLink(reqBody);

    nlohmann::json resj = nlohmann::json::parse(res);
    nlohmann::json target = resj["data"]["attendance_intervals_aggregate"]["aggregate"]["max"];

    if (target["ext"] != nullptr)
        sinceDate = target["ext"];
    else 
        sinceDate = "2010-01-01T00:00:00";

    time_t a = utils::str2unixtime(sinceDate, "%Y-%m-%dT%H:%M:%S");

    return a;
}

std::string GQLTransmitter::prepareGQLRequest(const string request)
{
    std::vector<char> result;
    int i = 0;
    for (char s: request){
        if (s != '\n' && s != '\t'){
            result.push_back(s);
            i++;
        }
    }
    std::string s(result.begin(), result.end());
    return s;
}

size_t CurlWrite_CallbackFunc_StdString(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size*nmemb;
    try
    {
        s->append((char*)contents, newLength);
    }
    catch(std::bad_alloc &e)
    {
        //handle memory problem
        return 0;
    }
    return newLength;
}

std::string GQLTransmitter::GQLhttpLink(const std::string &payload)
{

    CURL *curl;
    CURLcode res;

    /* In windows, this will init the winsock stuff */ 
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */ 
    curl = curl_easy_init();
    std::string s_responce;

    if(curl) {
        struct curl_slist *headers = NULL;

        headers = curl_slist_append(headers,  HASURA_SECRET_HEADER);
        headers = curl_slist_append(headers,  "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s_responce);

        /* First set the URL that is about to receive our POST. This URL can
        just as well be a https:// URL if that is what should receive the
        data. */ 
        curl_easy_setopt(curl, CURLOPT_URL, GRAPHQL_ENDPOINT);
        /* Now specify the POST data */ 
        // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);


        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str() );
        /* Perform the request, res will get the return code */ 
        res = curl_easy_perform(curl);
        /* Check for errors */ 
        if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        /* always cleanup */ 
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();
    cout << endl;
    return s_responce;
}

#define WORKSHIFT 50400

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "utils.hpp"
#include "types.hpp"
#include "dataPreparation.hpp"

using namespace std;


void transformDatabasetoCSV(){ 
    string zk_database_path = utils::getZkDatabasePath();

    string mdb_path = zk_database_path + "access.mdb";
    string result_users =  zk_database_path + "users.csv";
    string result_events =  zk_database_path + "events.csv";

    string cmd_u = "mdb-export " + mdb_path + " USERINFO > " + result_users;
    const char *command_u = cmd_u.c_str();
    system(command_u);

    string cmd_e = "mdb-export " + mdb_path + " acc_monitor_log > " + result_events;
    const char *command_e = cmd_e.c_str();
    system(command_e);    
}

vector<Event> parseEvents(){
    vector<Event> pool;
    pool.reserve(30000);
    
    string zk_database_path = utils::getZkDatabasePath();

    string path_csv = zk_database_path + "events.csv";
    ifstream filein(path_csv);

    if (filein.is_open()){
        for (string line; getline(filein, line);){

        vector<string> v = utils::split(line, ",");

        string id   = utils::removeQuotes(v[0]);
        string time = utils::removeQuotes(v[9]);
        string card = utils::removeQuotes(v[11]);
        string pointName= utils::removeQuotes(v[14]);
        
        char *end;
        time_t t = utils::str2unixtime(time, "%m/%d/%y %H:%M:%S");
        Event ev( strtoull(id.c_str(), &end, 10) , t, card, pointName);
        pool.push_back(ev);
        }
    } 
    else {
        cout << "Can't find the csv table with events."<< endl;
        exit(1);
    }   
    cout << "Parsed " << pool.size() << " events." << endl;
    return pool;
}

vector<Employee> parseEmployees(vector<Event> vec_events)
{
    vector<Employee> pool;
    std::string zk_database_path = utils::getZkDatabasePath();

    string path_csv = zk_database_path + "users.csv";
    ifstream filein(path_csv);

    if (!filein.is_open()) {
        cout << "Can't find the csv table with users."<< endl;
        exit(1);
    };
    int n = true;

    for (string line; getline(filein, line);){
        if (n) {
            n = false;
            continue;
        }

        vector<string> v = utils::split(line, ",");

        string card = utils::removeQuotes(v[38]);
        string first_name= utils::removeQuotes(v[46]);
        string last_name= utils::removeQuotes(v[3]);

        // Добавим все ивены для этого юзера
        vector<Event> events;    
        vector<Event>::iterator ev;
        for (ev = vec_events.begin(); ev != vec_events.end(); ev++){
            if (ev->card == card)
                events.push_back(*ev);
        }
        
        Employee user(card, first_name, last_name, events);
        user.unmodified_events = events;
        pool.push_back(user);
    }
    cout << "Parsed " << pool.size() << " users." << endl;
    return pool;
}


vector<Event> primarySampling(vector<Event> &vec, const time_t startSamplingDate ){
    vector<Event> sampled_vec;
    sampled_vec.reserve(1000);
   
    cout << "Starts primrarly sampling from " <<  utils::unixtime2str(startSamplingDate) << endl;
    
    for (auto &event: vec){
        if (event.card != "0" && event.time > startSamplingDate){
            sampled_vec.push_back(event);
        }
    }

    vec.clear();
    vec = sampled_vec;
    cout << "Primary sample complited. count events: " << vec.size() << endl;
    return vec;
}

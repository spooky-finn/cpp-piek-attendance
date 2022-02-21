#define WORKSHIFT 50400

#include <iostream>
#include <iomanip>
#include <sstream>

#include <string>
#include <vector>
#include <fstream>
#include <ctime> 

#include "utils.hpp"
#include "GQLTransmitter.hpp"
#include "dataPreparation.hpp"
#include "DataLogger.hpp"

using namespace std;
using namespace utils;

void employeeSinc(vector<Employee> &setA, GQLTransmitter &transmitter){
    nlohmann::json setB = transmitter.getEmployees();
    vector<Employee> setC;
    vector<Employee> setD;
    
    // setA - обрабатываемые данные - источник правды
    // setB - hasura
    // setC = setA - setB --> sending new to hasura (should_upload)
    // setD = setB - setA --> delete from hasura

    for (const auto &A: setA){

        bool added_yet = false;
        for (const auto &B: setB){
            
            if (A.card == B["card"]){
                added_yet = true;
                break;
            }
        }
        if (!added_yet)
            setC.push_back(A);
    }

    for (const auto &B: setB){
        bool shld_del = true;
        for (const auto &A: setA){
            
            if (A.card == B["card"]){
                shld_del = false;
                break;
            }
        }
        vector<Event> ev;
        if (shld_del)
            setD.push_back( Employee(B["card"], B["firstname"], B["lastname"], ev) );
    }

    transmitter.insertEmployee(setC);
    transmitter.deleteEmployee(setD);
}


int main(int const argc, const char* const argv[]){

    GQLTransmitter transmitter;
    DataLogger dataLogger;

    //------ settings parametres ----------//
    std::string executable_path = string(argv[0]);
    utils::getRootDir( executable_path );

    std::string zk_database_path;
    ifstream filein( std::string(CONFIG_PATH) + "database_path");
    if (!filein.is_open()) {
        cout << "Can`t find config file with path to zk teco database" << endl;
        exit(1);
    }
    filein >> zk_database_path;
    utils::getZkDatabasePath(zk_database_path);
        

        
    // it creates two csv file as a result.
    transformDatabasetoCSV();    
    unsigned int start_time = clock();

    vector<Event> events_pool =  parseEvents();


    time_t sinceDate  = transmitter.getlatestEventTimestamp();
    cout << "Resived timestamp of last event: " << utils::unixtime2str(sinceDate) << endl;
    time_t startSamplingDate = utils::addTimeToTime( sinceDate , -30);

    primarySampling(events_pool, startSamplingDate);

    vector<Employee> users = parseEmployees(events_pool);

    dataLogger.exportUnmodifiedEvents(users);
    employeeSinc(users, transmitter);

    transmitter.insertIntervals(users);

    dataLogger.exportFullyReport(users);

    unsigned int end_time = clock();
    cout << "total time " << (end_time - start_time ) * 1000.0 /  CLOCKS_PER_SEC << "ms \n" << endl;
    
    return 0;
}




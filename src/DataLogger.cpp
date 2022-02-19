#include <iostream>
#include <iomanip>

#include <string>
#include <vector>
#include <fstream>

#include "DataLogger.hpp"
#include "utils.hpp"

#define ROOT_DIR "/Employees/fin/Documents/etu-compscience/debagtest/"


void DataLogger::exportUnmodifiedEvents(vector<Employee> employess){
    string zk_database_path = utils::getZkDatabasePath();
    ofstream fout( zk_database_path + "unmodified_events.txt" );

    if (!fout.is_open()){
        cout << "\n Cannot open the report file" << endl;
        exit(1);
    }

    for (Employee user: employess){
        fout << "\n\n"  << string(40, '-') << endl;
        fout
        << user.first_name << " " 
        << user.last_name  << " "
        << user.card       << endl;

        fout << "EVENTS:" << endl;
        fout << string(40, '-') << endl;
        fout << " id | time | direction | event point name" << endl;
        fout << "this events passed anticollision alghoritm yet " << endl;
        fout << endl;

        for (auto ev: user.unmodified_events){
            fout << ev.id << "  "
            << utils::unixtime2str(ev.time) << "  " 
            << ev.dir << "  "
            << ev.pointName
            << endl;
        }

        fout << string(40, '-') << endl;
    }
}


void DataLogger::exportFullyReport(vector<Employee> vec_users)
{
    ofstream fout( std::string(ROOT_DIR) + "processing_report.txt");

    if (!fout.is_open()){
        cout << "\n Cannot open the report file" << endl;
        exit(1);
    }

    vector<Employee>::iterator user;

    for(user = vec_users.begin(); user != vec_users.end(); user++)
    {
        fout << "\n\n"  << string(40, '-') << endl;
        fout
        << user->first_name << " " 
        << user->last_name  << " "
        << user->card       << endl;


        fout << "EVENTS:" << endl;
        fout << string(40, '-') << endl;
        fout << " id | time | direction | event point name" << endl;
        fout << "this events passed anticollision alghoritm yet " << endl;
        fout << endl;


        vector<Event>::iterator ev;
        for (ev = user->events.begin(); ev != user->events.end(); ev++)
        {
            fout
            << ev->id << "  "
            << utils::unixtime2str(ev->time) << "  " << ev->dir << "  "
            << ev->pointName
            << endl;
        }        

        fout << string(40, '-') << endl;

        fout << "INTERVALS:" << endl;
        fout << string(40, '-') << endl;
        vector<Interval>::iterator intr;
        for (intr = user->intervals.begin(); intr != user->intervals.end(); intr++)
        {
            fout << utils::unixtime2str(intr->ent) << " " 
            << utils::unixtime2str(intr->ext) << " " 
            << "-- "
            << intr->dur / 60 
            << "m ("
            << (intr->dur / 3600.0)
            << "h)"
            <<  endl;
        }
        fout << string(40, '-') << endl;
    }    

    fout << endl;
}

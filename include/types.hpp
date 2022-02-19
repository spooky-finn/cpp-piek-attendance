#pragma once
#include <string>
#include <vector>
using namespace std;


struct Event{
    Event(int64_t _id, time_t _time, std::string _card, std::string _pointName){
        id = _id;
        time = _time;
        card = _card;
        pointName = _pointName;
    };

    int64_t id;
    time_t time;
    std::string card;
    std::string dir;
    std::string pointName;
};


struct Interval{
    Interval(time_t _ent, time_t _ext, std::string _card, unsigned long _dur){
        ent = _ent;
        ext = _ext;
        card = _card;
        dur = _dur;
    };

    time_t ent;
    time_t ext;
    std::string card;
    unsigned long dur;
};


class Employee {
public:
    Employee(){};
    Employee(string _card, string _first_name, string _last_name, vector<Event> _events){
        card = _card;
        first_name = _first_name;
        last_name  = _last_name;
        events     = _events;

        sortEvents();

        // there is method correcting the incoming events and changing them
        preventCollision();

        marking(events);
        intermediateSampling();

        composition();
        // intervalAnalysis(intervals);
    };

    string card;
    string first_name;
    string last_name;

    vector<Event> events;
    vector<Event> unmodified_events; 
    vector<Interval> intervals;
    vector<Interval> reviewed_intervals;

private:
    template<typename T>
    static bool compareByTimestamp(const T &a, const T &b);

    void sortEvents();
    vector<Event>::iterator r_preventCollision(vector<Event>::iterator current_event);
    void preventCollision();

    // |------------------------------------------------------------------------------------------------
    // | marking events by type (ent or ext)
    // | based on the module distance to the previous event
    // |-------------------------------------------------------------------------------------------------
    void marking(vector<Event> &events);
    void intermediateSampling();

    //   | Collects interval based on marking events
    //   |------------------------------------------------------------------------------------------------
    //   |В этом классе интревал обозначает объект с данными о приходе и уходе
    //   |Интервал всегда начаинает с прихода, поэтому возможны два случая
    //   |  enr 
    //   |  enr
    //   |->  такой интервал мы регистрируем, но ставим duration в 0, тк это проеб сотрудника
    //   |  
    //   |  ent
    //   |  ext
    //   |->  хороший интервал, то что нам надо :')
    //   |-------------------------------------------------------------------------------------------------
    void composition();

    
    void intervalAnalysis(vector<Interval> &intervals);
};
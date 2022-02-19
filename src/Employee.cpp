#define WORKSHIFT 50400

#include <iostream>
#include <vector>
#include <string>

#include "types.hpp"
#include "utils.hpp"
#include "GQLTransmitter.hpp"

using namespace std;

template<typename T>
bool Employee::compareByTimestamp(const T &a, const T &b){
    return a.time < b.time;
}

void Employee::sortEvents(){
    // cout << "before collision refused length is " << this->events.size() << endl;
    // coutEvents(this->events);
    std::sort(this->events.begin(), this->events.end(), compareByTimestamp<Event>);
}

// Since func is recursive, it returns a valid and unique event from certain delta interval
vector<Event>::iterator Employee::r_preventCollision(vector<Event>::iterator current_event){
    auto nx = next(current_event);
    // check if we are in the aviable memory sector.
    if (nx == this->events.end() ) return current_event;
    
    const long timedelta = utils::getTimedelta(current_event->time, nx->time);  //result in seconds
    if (timedelta == -1) return current_event;

    // 300 seconds a non-sensitive time for double  marks.
    if (timedelta < 300)
        return r_preventCollision( nx );
    else
        return current_event;
}

// |  Anthocollision algorithm
// |------------------------------------------------------------------------------------------------
// | Anthocollision algorithm, descends into primary sampling algorithm
// | The essence of the algorithm is that if a person has 2 events at +- the same time
// | then the algorithm recursively skips their similar ones so as not to spoil the statistics
// |-------------------------------------------------------------------------------------------------
void Employee::preventCollision(){
    vector<Event> revisited;
    revisited.reserve(this->events.size());
    vector<Event>::iterator ev;
    // cout << "Until conflict are resolved. length id " << events.size() << endl;
    // coutEvents(this->events);

    ev = this->events.begin();

    while( ev != this->events.end() ){
        // Since func is recursive, it returns a valid and unique event from certain delta interval
        vector<Event>::iterator resultedEvent = r_preventCollision(ev);
        revisited.push_back(*resultedEvent);
        ev = resultedEvent + 1;
    }
    this->events = revisited;
    // cout << "After conflict are resolved. length id " << debaggedevents.size() << endl;
    // coutEvents(this->debaggedevents);
}

// |------------------------------------------------------------------------------------------------
// | marking events by type (ent or ext)
// | based on the module distance to the previous event
// |-------------------------------------------------------------------------------------------------
void Employee::marking(vector<Event> &events){
    vector<Event>::iterator ev;

    for (ev = events.begin(); ev != events.end(); ev++)
    {
        auto nx = next(ev);
        // Сделам проверку, чтобы не выйти за пределы доступа памяти
        // Core Dump (Segmentation fault)
        if (nx == events.end() ) break;

        long timedelt = utils::getTimedelta(ev->time, nx->time);  
        if (timedelt == -1) continue;

        if (ev == events.begin())
            ev->dir = "ent";

        if (timedelt < WORKSHIFT && ev->dir == "ent"){
            nx->dir = "ext";
        }
        else{
                nx->dir = "ent";
        }
    }
}

// |------------------------------------------------------------------------------------------------
// |  In the primary sampling method we are minus several day from 'since' timestamp
// |  Now after marking events we are should undercut the sample a little
// |-------------------------------------------------------------------------------------------------
void Employee::intermediateSampling(){
    vector<Event> res;
    GQLTransmitter transmitter;
    time_t sinceDate = transmitter.getlatestEventTimestamp();

    for (auto ev : this->events){
        if (ev.time &&  ev.time > sinceDate)
            res.push_back(ev);
    }
    this->events = res;
    this->events.reserve(this->events.size());
    cout << "Intermidate sampling done. " << this->events.size() << " events after" << endl;
}

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
void Employee::composition(){
    vector<Event>::iterator ev;

    for (ev = this->events.begin(); ev != this->events.end(); ev++)
    {
        auto nx = next(ev);
        // Сделам проверку, чтобы не выйти за пределы доступа памяти
        // Core Dump (Segmentation fault)
        if (nx == this->events.end()) break;

        if (ev->dir == "ent" && nx->dir == "ext"){

            Interval interv( ev->time, nx->time, this->card, utils::getTimedelta(ev->time, nx->time) );
            this->intervals.push_back(interv);
        }

        else if (ev->dir == "ent" && nx->dir == "ent") {
            Interval interv( ev->time, 0, this->card, 0);
            this->intervals.push_back(interv);
        }
    }
}

// deprecated
void Employee::intervalAnalysis(vector<Interval> &intervals){
    cout << "--" << first_name << " " << last_name <<endl;

    vector<Interval>::iterator ev;
    vector<Interval>::iterator inner_ev;


    for (ev = intervals.begin(); ev != intervals.end(); ev++){
        std::string curdate = utils::split(utils::unixtime2str(ev->ent), "T")[0];
        // std::string nextdate = utils::split(utils::unixtime2str(nx->ent), "T")[0];

        size_t should_skip = 0;
        unsigned long long combined_duration = ev->dur;
        vector<Interval> vec_should_combine;
        vec_should_combine.push_back(*ev);
        
        auto nx = next(ev);
        if (nx == this->intervals.end()){
            this->reviewed_intervals.push_back(*ev);
            break;
        }

        // в цикле проверяем следующие, за текущим, интервалы и если у следующего совпадает день, то добавляем его
        for (inner_ev = nx; inner_ev != intervals.end(); inner_ev++){

            // дата ивента во внетреннем цикле
            std::string inner_evdate = utils::split(utils::unixtime2str(inner_ev->ent), "T")[0];

            if (curdate == inner_evdate){
                vec_should_combine.push_back(*inner_ev);
                combined_duration += inner_ev->dur;
                should_skip++;
            }
            // This mean that interval only one on this day.
            else break;
        }

        if (vec_should_combine.size() > 1){
            Interval first = *vec_should_combine.begin();
            Interval last = *(vec_should_combine.end()-1);

            Interval combined(first.ent, last.ext, first.card, combined_duration);
            ev += should_skip;
            this->reviewed_intervals.push_back(combined);
        }
        else 
            this->reviewed_intervals.push_back(*ev);
        
    }

    cout << "\n ORIGINAL INTERVALS " <<  this->intervals.size() << endl;
    for (auto intr: this->intervals){
        cout << utils::unixtime2str(intr.ent) << " " << utils::unixtime2str(intr.ext) << " " << intr.ext << "__" << intr.dur << endl;
    }

    cout << "\n REVIEWED INTERVALS " <<  this->reviewed_intervals.size() << endl;
    for (auto intr: this->reviewed_intervals){
        cout << utils::unixtime2str(intr.ent) << " " << utils::unixtime2str(intr.ext) << " " << intr.ext << "__" << intr.dur << endl;
    }
}

// void Employee::intervalAnalysis(){
//     vector<Interval>::iterator ev;

//     cout << '\n' << endl;
//     cout << this->first_name << " " << this->last_name << endl;

//     for (ev = this->intervals.begin(); ev != this->intervals.end(); ev++)
//     {
//         cout << "cur ev " << utils::unixtime2str(ev->ent) << " " << utils::unixtime2str(ev->ext) << endl;
//         auto nx = next(ev);
//         // Сделам проверку, чтобы не выйти за пределы доступа памяти
//         // Core Dump (Segmentation fault)
//         if (nx == this->intervals.end()) break;

//         // |------------------------------------------------------------------------------------------------
//         // |   В целях оптимизации, ищем паттерн, когда 2 интервала принадлежат одиному календарному дню
//         // |   Если он найден то просмотрим все интервалы,
//         // |   может таких там 3 или 5, чтобы учесть все
//         // |
//         // |-------------------------------------------------------------------------------------------------
                
//         struct std::tm * timeinfo_ev;
//         struct std::tm * timeinfo_nx;
//         time_t cur_time = ev->ent;
//         time_t next_tme = nx->ext;
        
//         timeinfo_ev = localtime(&next_tme);
//         timeinfo_nx = localtime(&cur_time);
//         cout << "L: "<< ev->ent << " " <<  nx->ent << endl;
//         cout << "LL: "<< asctime(timeinfo_ev) << " " <<  asctime(timeinfo_nx) << endl;

//         if ( timeinfo_ev->tm_mday == timeinfo_nx->tm_mday){
//             cout << "!!" << timeinfo_ev->tm_mday << " " << timeinfo_nx->tm_mday << endl;
        
//             vector<Interval> vec_should_combine;
//             unsigned long duration = 0;

//             int day_search = timeinfo_ev->tm_mday;
//             int month_search = timeinfo_ev->tm_mon;
//             int year_search =  timeinfo_ev->tm_year;

//             vector<Interval>::iterator innerev;

//             size_t should_skip = 0;

//             for (const auto &innerev : this->intervals)
//             {   
//                 struct std::tm * timeinfo3;
//                 timeinfo3 = localtime(&innerev.ent);

//                 if ( timeinfo3->tm_mday == day_search && timeinfo3->tm_mon == month_search && timeinfo3->tm_year == year_search){
//                     // если такоые находятся, то нам нужно учесть их количество, 
//                     // чтобы на следущей итерации цикла мы не стали их проходить
//                     duration +=innerev.dur;
//                     should_skip++;
//                     vec_should_combine.push_back(innerev);
//                 }
//             }

//             Interval first = *vec_should_combine.begin();
//             Interval last = *(vec_should_combine.end()-1);

//             Interval combined(first.ent, last.ext, first.card, duration);

//             this->reviewed_intervals.push_back(combined);

//             if (should_skip)
//                 ev += should_skip-1;
//         }
//         else 
//             this->reviewed_intervals.push_back(*ev);
//     }
// }

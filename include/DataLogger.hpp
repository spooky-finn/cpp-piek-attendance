#include <vector>
#include "types.hpp"

class DataLogger{
public:
    DataLogger(){};
    void exportFullyReport(vector<Employee> vec_users);
    void exportUnmodifiedEvents(vector<Employee> employess);
};
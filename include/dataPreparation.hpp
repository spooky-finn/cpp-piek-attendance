#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "types.hpp"

void transformDatabasetoCSV();

std::vector<Event> parseEvents();
std::vector<Employee> parseEmployees(std::vector<Event> vec_events);

vector<Event> primarySampling(vector<Event> &vec, const time_t startSamplingDate);

void exportUnmodifiedEvents(vector<Employee> employess);
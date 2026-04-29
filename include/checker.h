#pragma once
#include "data_types.h"
#include <vector>
#include <string>

namespace Checker 
{
    struct Problem 
    {
        std::string kind;
        std::string detail;
    };
    struct Result 
    {
        std::vector<Problem> problems;
        int hardCount();
        int softCount();
        int score();
    };
    Result checkAll(std::vector<ScheduleEntry>& s, InputData& data);
    void   printResult(Result& r);
}

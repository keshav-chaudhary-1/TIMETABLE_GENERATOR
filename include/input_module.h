#pragma once
#include "data_types.h"
#include <string>

namespace InputModule {
    InputData loadFromCSV(std::string teacherFile,
                          std::string subjectFile,
                          std::string roomFile,
                          int workingDays,
                          int periodsPerDay);
    void printSummary(InputData& data);
}

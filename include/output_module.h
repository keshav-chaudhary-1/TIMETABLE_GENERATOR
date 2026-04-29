#pragma once
#include "data_types.h"
#include <vector>
#include <string>

namespace Output {
    // Student view: one table per section
    void printSectionWise  (std::vector<ScheduleEntry>& s, InputData& data);

    // Teacher view: one table per teacher
    void printTeacherWise  (std::vector<ScheduleEntry>& s, InputData& data);

    // Room view: what is happening in each room each slot
    void printRoomWise     (std::vector<ScheduleEntry>& s, InputData& data);

    // Workload: how many classes each teacher has
    void printWorkload     (std::vector<ScheduleEntry>& s, InputData& data);

    // Export everything to separate CSV files
    void exportAllCSV      (std::vector<ScheduleEntry>& s, InputData& data, int version);
}

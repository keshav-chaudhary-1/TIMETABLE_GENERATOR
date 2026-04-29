#pragma once
#include "data_types.h"
#include "checker.h"
#include <vector>

namespace Optimizer {
    std::vector<ScheduleEntry> improve(
        std::vector<ScheduleEntry>& schedule,
        InputData& data,
        int iterations = 15000);
}

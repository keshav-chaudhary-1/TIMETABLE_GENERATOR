#pragma once
#include "data_types.h"
#include "conflict_graph.h"
#include <vector>

namespace Scheduler {
    bool assignSlots(ConflictGraph::Graph& g, InputData& data);
    bool assignRooms (ConflictGraph::Graph& g, InputData& data);
    std::vector<ScheduleEntry> toSchedule(ConflictGraph::Graph& g, InputData& data);
}

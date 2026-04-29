#pragma once
#include "data_types.h"
#include <vector>

namespace ConflictGraph {
    struct Graph {
        std::vector<LectureNode>        nodes;
        std::vector<std::vector<bool>>  hasConflict;
    };

    Graph build(InputData& data);
    void  printInfo(Graph& g);
}

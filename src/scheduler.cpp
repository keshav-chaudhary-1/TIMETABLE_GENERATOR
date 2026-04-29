#include "../include/scheduler.h"
#include <iostream>
#include <cstdlib>

namespace Scheduler {

bool assignSlots(ConflictGraph::Graph& g, InputData& data) {
    int n        = (int)g.nodes.size();
    int numSlots = (int)data.slots.size();

    // Sort nodes by degree (most conflicted first)
    std::vector<int> degree(n, 0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (g.hasConflict[i][j]) degree[i]++;

    std::vector<int> order;
    for (int i = 0; i < n; i++) order.push_back(i);
    for (int i = 0; i < n; i++)
        for (int j = i+1; j < n; j++)
            if (degree[order[j]] > degree[order[i]]) {
                int tmp = order[i]; order[i] = order[j]; order[j] = tmp;
            }

    // Greedy slot assignment with random start for regeneration variety
    for (int idx = 0; idx < n; idx++) {
        int u = order[idx];

        std::vector<bool> forbidden(numSlots, false);
        for (int v = 0; v < n; v++)
            if (g.hasConflict[u][v] && g.nodes[v].assignedSlot != -1)
                forbidden[g.nodes[v].assignedSlot] = true;

        int startSlot = rand() % numSlots;
        int assigned  = -1;
        for (int k = 0; k < numSlots; k++) {
            int slot = (startSlot + k) % numSlots;
            if (!forbidden[slot]) { assigned = slot; break; }
        }

        if (assigned == -1) {
            std::cout << "  Warning: could not place node " << u << "\n";
            // Fallback
            for (int s = 0; s < numSlots; s++)
                if (!forbidden[s]) { assigned = s; break; }
            if (assigned == -1) return false;
        }
        g.nodes[u].assignedSlot = assigned;
    }
    return true;
}

bool assignRooms(ConflictGraph::Graph& g, InputData& data) {
    int n = (int)g.nodes.size();
    // usedRooms[slotId] = list of room ids taken
    std::vector<std::vector<int>> usedRooms(data.slots.size());

    for (int i = 0; i < n; i++) {
        LectureNode& node = g.nodes[i];
        int  slot         = node.assignedSlot;
        bool needLab      = node.requiresLab;
        int  assigned     = -1;

        for (int r = 0; r < (int)data.rooms.size(); r++) {
            if (data.rooms[r].isLab != needLab) continue;
            bool taken = false;
            for (int used : usedRooms[slot])
                if (used == data.rooms[r].id) { taken = true; break; }
            if (!taken) { assigned = data.rooms[r].id; break; }
        }

        // Fallback: any free room
        if (assigned == -1) {
            for (int r = 0; r < (int)data.rooms.size(); r++) {
                bool taken = false;
                for (int used : usedRooms[slot])
                    if (used == data.rooms[r].id) { taken = true; break; }
                if (!taken) { assigned = data.rooms[r].id; break; }
            }
        }

        if (assigned == -1) {
            std::cout << "  Warning: no room for node " << i << "\n";
            return false;
        }
        node.assignedRoom = assigned;
        usedRooms[slot].push_back(assigned);
    }
    return true;
}

std::vector<ScheduleEntry> toSchedule(ConflictGraph::Graph& g, InputData& data) {
    std::vector<ScheduleEntry> schedule;
    for (int i = 0; i < (int)g.nodes.size(); i++) {
        LectureNode& node = g.nodes[i];
        ScheduleEntry e;
        e.subjectId  = node.subjectId;
        e.section    = node.section;
        e.roomId     = node.assignedRoom;
        e.slotId     = node.assignedSlot;
        e.isLab      = node.requiresLab;
        e.teacherIds = node.teacherIds;
        schedule.push_back(e);
    }
    return schedule;
}

} // namespace Scheduler

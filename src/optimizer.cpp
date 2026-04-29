#include "../include/optimizer.h"
#include <cstdlib>
#include <iostream>

namespace Optimizer {

std::vector<ScheduleEntry> improve(
    std::vector<ScheduleEntry>& schedule,
    InputData& data,
    int iterations) {

    std::vector<ScheduleEntry> best    = schedule;
    std::vector<ScheduleEntry> current = schedule;
    int bestScore    = Checker::checkAll(best,    data).score();
    int currentScore = Checker::checkAll(current, data).score();

    int n        = (int)current.size();
    int numSlots = (int)data.slots.size();

    for (int iter = 0; iter < iterations; iter++) {

        int moveType = rand() % 2;

        if (moveType == 0) {
            // Move: change one entry's slot randomly
            int i       = rand() % n;
            int newSlot = rand() % numSlots;
            int oldSlot = current[i].slotId;
            current[i].slotId = newSlot;

            int newScore = Checker::checkAll(current, data).score();
            if (newScore <= currentScore) {
                currentScore = newScore;
                if (newScore < bestScore) {
                    bestScore = newScore;
                    best      = current;
                }
            } else {
                current[i].slotId = oldSlot; // revert
            }

        } else {
            // Move: swap slots of two random entries
            int i = rand() % n;
            int j = rand() % n;
            if (i == j) continue;

            int slotI = current[i].slotId;
            int slotJ = current[j].slotId;
            current[i].slotId = slotJ;
            current[j].slotId = slotI;

            int newScore = Checker::checkAll(current, data).score();
            if (newScore <= currentScore) {
                currentScore = newScore;
                if (newScore < bestScore) {
                    bestScore = newScore;
                    best      = current;
                }
            } else {
                // revert
                current[i].slotId = slotI;
                current[j].slotId = slotJ;
            }
        }

        if (bestScore == 0) break;
    }

    std::cout << "  Optimizer done. Best score: " << bestScore << "\n";
    return best;
}

} // namespace Optimizer

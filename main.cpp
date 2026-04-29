#include "include/input_module.h"
#include "include/conflict_graph.h"
#include "include/scheduler.h"
#include "include/checker.h"
#include "include/optimizer.h"
#include "include/output_module.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

std::vector<ScheduleEntry> generateTimetable(InputData& data) {

    // Step 1: Build conflict graph
    std::cout << "\n  [1] Building conflict graph...\n";
    ConflictGraph::Graph g = ConflictGraph::build(data);
    ConflictGraph::printInfo(g);

    // Step 2: Assign time slots (greedy coloring)
    std::cout << "\n  [2] Assigning time slots...\n";
    bool ok = Scheduler::assignSlots(g, data);
    if (!ok) { std::cout << "  ERROR: Slot assignment failed.\n"; return {}; }

    // Step 3: Assign rooms
    std::cout << "\n  [3] Assigning rooms...\n";
    ok = Scheduler::assignRooms(g, data);
    if (!ok) { std::cout << "  ERROR: Room assignment failed.\n"; return {}; }

    // Step 4: Convert to schedule list
    std::vector<ScheduleEntry> schedule = Scheduler::toSchedule(g, data);
    std::cout << "  Schedule entries: " << schedule.size() << "\n";

    // Step 5: Check initial quality
    std::cout << "\n  [4] Initial constraint check...\n";
    Checker::Result check = Checker::checkAll(schedule, data);
    Checker::printResult(check);

    // Step 6: Optimize
    std::cout << "\n  [5] Optimizing...\n";
    schedule = Optimizer::improve(schedule, data, 20000);

    // Step 7: Final check
    std::cout << "\n  [6] Final constraint check...\n";
    check = Checker::checkAll(schedule, data);
    Checker::printResult(check);

    return schedule;
}

void printMenu() {
    std::cout << "\n================================================\n";
    std::cout << "  What would you like to do?\n";
    std::cout << "  [1] View section-wise timetable\n";
    std::cout << "  [2] View teacher-wise timetable\n";
    std::cout << "  [3] View room-wise timetable\n";
    std::cout << "  [4] View workload report\n";
    std::cout << "  [5] Export all to CSV files\n";
    std::cout << "  [R] Regenerate new timetable\n";
    std::cout << "  [Q] Quit\n";
    std::cout << "================================================\n";
    std::cout << "  Enter choice: ";
}

int main() {
    srand((unsigned int)time(0));

    std::cout << "\n";
    std::cout << "================================================\n";
    std::cout << "   CSE 2nd Year Timetable Generator\n";
    std::cout << "   Team TIME HACKER | DAA IV-T008\n";
    std::cout << "   Graphic Era Deemed to be University\n";
    std::cout << "================================================\n";

    // Load CSV data
    std::cout << "\n[Loading] Reading CSV files from data/ folder...\n";
    InputData data = InputModule::loadFromCSV(
        "data/teachers.csv",
        "data/subjects.csv",
        "data/rooms.csv",
        6,   // Mon to Sat
        8    // 8 periods per day (8:00 AM to 4:05 PM)
    );
    InputModule::printSummary(data);

    int attempt = 1;
    std::vector<ScheduleEntry> schedule;

    // First generation
    std::cout << "\n[Generating] Attempt " << attempt << "...\n";
    schedule = generateTimetable(data);

    while (true) {
        if (schedule.empty()) {
            std::cout << "\nGeneration failed. Retrying...\n";
            attempt++;
            schedule = generateTimetable(data);
            continue;
        }

        printMenu();
        std::string choice;
        std::cin >> choice;
        if (!choice.empty()) choice[0] = tolower(choice[0]);

        if (choice == "1") {
            Output::printSectionWise(schedule, data);

        } else if (choice == "2") {
            Output::printTeacherWise(schedule, data);

        } else if (choice == "3") {
            Output::printRoomWise(schedule, data);

        } else if (choice == "4") {
            Output::printWorkload(schedule, data);

        } else if (choice == "5") {
            std::cout << "\n  Exporting CSV files...\n";
            Output::exportAllCSV(schedule, data, attempt);

        } else if (choice == "r") {
            attempt++;
            std::cout << "\n[Regenerating] Attempt " << attempt << "...\n";
            schedule = generateTimetable(data);

        } else if (choice == "q") {
            std::cout << "\n  Goodbye!\n\n";
            break;

        } else {
            std::cout << "  Invalid choice. Try again.\n";
        }
    }

    return 0;
}

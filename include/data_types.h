#pragma once
#include <string>
#include <vector>

// ── Core data types ────────────────────────────────────────────────

struct Teacher {
    int         id;
    std::string name;
};

struct Subject {
    int         id;
    std::string code;
    std::string name;
    int         hoursPerWeek;
    bool        requiresLab;
    std::string section;               // A, B, C ... H

    // Multiple teachers: e.g. "Dr. Rehan & Mr. Abhishek"
    std::vector<int> teacherIds;       // theory teachers
    std::vector<int> labTeacherIds;    // lab teachers (may be different)
};

struct Room {
    int         id;
    std::string name;
    bool        isLab;
    int         capacity;
};

struct TimeSlot {
    int         id;
    int         day;      // 0=Mon ... 5=Sat
    int         period;   // 0-based

    std::string dayName() {
        std::string d[] = {"Mon","Tue","Wed","Thu","Fri","Sat"};
        return d[day];
    }
    std::string toString() {
        return dayName() + "-P" + std::to_string(period + 1);
    }
};

// One scheduled entry
struct ScheduleEntry {
    int         subjectId;
    std::string section;
    int         roomId;
    int         slotId;
    bool        isLab;
    // The teacher(s) actually taking this slot
    std::vector<int> teacherIds;
};

// One node in the conflict graph = one lecture hour to be placed
struct LectureNode {
    int         nodeId;
    int         subjectId;
    std::string section;
    bool        requiresLab;
    std::vector<int> teacherIds;   // all teachers involved
    int         assignedSlot = -1;
    int         assignedRoom = -1;
};

struct InputData {
    std::vector<Teacher>  teachers;
    std::vector<Subject>  subjects;
    std::vector<Room>     rooms;
    std::vector<TimeSlot> slots;
    int periodsPerDay = 8;
    int workingDays   = 6;   // Mon-Sat
};

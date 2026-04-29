#include "../include/output_module.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

namespace Output {

// ── Helpers ────────────────────────────────────────────────────────

// Build a display string for teacher list
std::string teacherNames(std::vector<int>& ids, InputData& data) {
    std::string result;
    for (int i = 0; i < (int)ids.size(); i++) {
        if (i > 0) result += " & ";
        // Use short name: last name only to save space
        std::string full = data.teachers[ids[i]].name;
        // Find last space
        int pos = (int)full.rfind(' ');
        result += (pos != (int)std::string::npos) ? full.substr(pos+1) : full;
    }
    return result;
}

// Get all unique sections in sorted order
std::vector<std::string> getSections(InputData& data) {
    std::vector<std::string> sections;
    for (int i = 0; i < (int)data.subjects.size(); i++) {
        std::string sec = data.subjects[i].section;
        bool found = false;
        for (int j = 0; j < (int)sections.size(); j++)
            if (sections[j] == sec) { found = true; break; }
        if (!found) sections.push_back(sec);
    }
    std::sort(sections.begin(), sections.end());
    return sections;
}

// Time labels for periods (matching real timetable)
std::string periodLabel(int p) {
    std::string labels[] = {
        "8:00-8:55",
        "9:00-9:55",
        "9:55-10:50",
        "11:10-12:05",
        "12:05-1:00",
        "1:00-1:55",
        "1:55-2:50",
        "3:10-4:05"
    };
    if (p < 8) return labels[p];
    return "P" + std::to_string(p+1);
}

// Print a horizontal separator line
void hline(int cols, int colW) {
    std::cout << "+" << std::string(10, '-');
    for (int i = 0; i < cols; i++)
        std::cout << "+" << std::string(colW, '-');
    std::cout << "+\n";
}

// ── Section-wise timetable ─────────────────────────────────────────

void printSectionWise(std::vector<ScheduleEntry>& s, InputData& data) {
    std::string dayNames[] = {"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
    std::vector<std::string> sections = getSections(data);
    int colW = 22;

    std::cout << "\n==========================================\n";
    std::cout << "   SECTION-WISE STUDENT TIMETABLE\n";
    std::cout << "   CSE 2nd Year | All Sections\n";
    std::cout << "==========================================\n";

    for (int si = 0; si < (int)sections.size(); si++) {
        std::string sec = sections[si];

        std::cout << "\n┌─────────────────────────────────────────────┐\n";
        std::cout << "│  Section: " << sec
                  << "   (CSE 2nd Year / 4th Sem)          │\n";
        std::cout << "└─────────────────────────────────────────────┘\n";

        // Print period header
        std::cout << std::left << std::setw(11) << "Day \\ Time";
        for (int p = 0; p < data.periodsPerDay; p++)
            std::cout << "│" << std::setw(colW) << periodLabel(p);
        std::cout << "│\n";
        hline(data.periodsPerDay, colW);

        for (int day = 0; day < data.workingDays; day++) {
            // Line 1: subject code + room
            std::cout << std::setw(11) << dayNames[day].substr(0,3);
            for (int per = 0; per < data.periodsPerDay; per++) {
                std::string cell = " FREE";
                for (int i = 0; i < (int)s.size(); i++) {
                    if (s[i].section != sec) continue;
                    if (data.slots[s[i].slotId].day    != day) continue;
                    if (data.slots[s[i].slotId].period != per) continue;
                    std::string code = data.subjects[s[i].subjectId].code;
                    std::string room = data.rooms[s[i].roomId].name;
                    cell = " " + code + " [" + room + "]";
                    if ((int)cell.size() > colW-1) cell = cell.substr(0, colW-1);
                    break;
                }
                std::cout << "│" << std::left << std::setw(colW) << cell;
            }
            std::cout << "│\n";

            // Line 2: teacher names
            std::cout << std::setw(11) << "";
            for (int per = 0; per < data.periodsPerDay; per++) {
                std::string cell = "";
                for (int i = 0; i < (int)s.size(); i++) {
                    if (s[i].section != sec) continue;
                    if (data.slots[s[i].slotId].day    != day) continue;
                    if (data.slots[s[i].slotId].period != per) continue;
                    cell = " " + teacherNames(s[i].teacherIds, data);
                    if ((int)cell.size() > colW-1) cell = cell.substr(0, colW-2) + ".";
                    break;
                }
                std::cout << "│" << std::left << std::setw(colW) << cell;
            }
            std::cout << "│\n";
            hline(data.periodsPerDay, colW);
        }
        std::cout << "\n";
    }
}

// ── Teacher-wise timetable ─────────────────────────────────────────

void printTeacherWise(std::vector<ScheduleEntry>& s, InputData& data) {
    std::string dayNames[] = {"Mon","Tue","Wed","Thu","Fri","Sat"};
    int colW = 20;

    std::cout << "\n==========================================\n";
    std::cout << "      TEACHER-WISE TIMETABLE\n";
    std::cout << "==========================================\n";

    for (int t = 0; t < (int)data.teachers.size(); t++) {

        // Check if this teacher has any classes
        bool hasAny = false;
        for (int i = 0; i < (int)s.size(); i++) {
            for (int tid : s[i].teacherIds) {
                if (tid == t) { hasAny = true; break; }
            }
            if (hasAny) break;
        }
        if (!hasAny) continue;

        std::cout << "\n  Teacher: " << data.teachers[t].name << "\n";
        std::cout << "  " << std::string(11 + data.periodsPerDay * (colW+1), '-') << "\n";

        // Header row
        std::cout << "  " << std::left << std::setw(9) << "Day";
        for (int p = 0; p < data.periodsPerDay; p++)
            std::cout << "│" << std::setw(colW) << periodLabel(p).substr(0,colW);
        std::cout << "│\n";
        std::cout << "  " << std::string(9, '-');
        for (int p = 0; p < data.periodsPerDay; p++)
            std::cout << "+" << std::string(colW, '-');
        std::cout << "+\n";

        for (int day = 0; day < data.workingDays; day++) {
            std::cout << "  " << std::setw(9) << dayNames[day];
            for (int per = 0; per < data.periodsPerDay; per++) {
                std::string cell = "-";
                for (int i = 0; i < (int)s.size(); i++) {
                    if (data.slots[s[i].slotId].day    != day) continue;
                    if (data.slots[s[i].slotId].period != per) continue;
                    bool found = false;
                    for (int tid : s[i].teacherIds)
                        if (tid == t) { found = true; break; }
                    if (!found) continue;
                    std::string code = data.subjects[s[i].subjectId].code;
                    std::string sec  = s[i].section;
                    std::string room = data.rooms[s[i].roomId].name;
                    cell = code + " Sec-" + sec + " " + room;
                    if ((int)cell.size() > colW-1) cell = cell.substr(0, colW-1);
                    break;
                }
                std::cout << "│" << std::left << std::setw(colW) << cell;
            }
            std::cout << "│\n";
        }
        std::cout << "\n";
    }
}

// ── Room-wise timetable ────────────────────────────────────────────

void printRoomWise(std::vector<ScheduleEntry>& s, InputData& data) {
    std::cout << "\n==========================================\n";
    std::cout << "         ROOM-WISE TIMETABLE\n";
    std::cout << "==========================================\n";

    for (int r = 0; r < (int)data.rooms.size(); r++) {
        Room& room = data.rooms[r];

        // Check if this room has anything scheduled
        bool hasAny = false;
        for (int i = 0; i < (int)s.size(); i++)
            if (s[i].roomId == room.id) { hasAny = true; break; }
        if (!hasAny) continue;

        std::cout << "\n  Room: " << room.name
                  << (room.isLab ? " [LAB]" : "      ")
                  << "  Capacity: " << room.capacity << "\n";
        std::cout << "  " << std::string(65, '-') << "\n";
        std::cout << "  " << std::left
                  << std::setw(12) << "Slot"
                  << std::setw(8)  << "Sec"
                  << std::setw(18) << "Subject"
                  << "Teachers\n";
        std::cout << "  " << std::string(65, '-') << "\n";

        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i].roomId != room.id) continue;
            std::cout << "  "
                << std::setw(12) << data.slots[s[i].slotId].toString()
                << std::setw(8)  << s[i].section
                << std::setw(18) << data.subjects[s[i].subjectId].code
                << teacherNames(s[i].teacherIds, data) << "\n";
        }
    }
    std::cout << "\n";
}

// ── Workload report ────────────────────────────────────────────────

void printWorkload(std::vector<ScheduleEntry>& s, InputData& data) {
    std::cout << "\n==========================================\n";
    std::cout << "       TEACHER WORKLOAD REPORT\n";
    std::cout << "==========================================\n";
    std::cout << "\n  " << std::left
              << std::setw(30) << "Teacher"
              << std::setw(15) << "Classes/Week"
              << "Bar\n";
    std::cout << "  " << std::string(65, '-') << "\n";

    for (int t = 0; t < (int)data.teachers.size(); t++) {
        int count = 0;
        for (int i = 0; i < (int)s.size(); i++)
            for (int tid : s[i].teacherIds)
                if (tid == t) { count++; break; }
        if (count == 0) continue;
        std::string bar(count, '#');
        std::cout << "  "
                  << std::setw(30) << data.teachers[t].name
                  << std::setw(15) << count
                  << bar << "\n";
    }
    std::cout << "\n";
}

// ── CSV Export ─────────────────────────────────────────────────────

void exportAllCSV(std::vector<ScheduleEntry>& s, InputData& data, int version) {

    // 1. Full schedule CSV
    std::string fname = "output_v" + std::to_string(version) + "_schedule.csv";
    std::ofstream f(fname);
    f << "Slot,Day,Period,Time,Section,SubjectCode,SubjectName,Teachers,Room,IsLab\n";
    for (int i = 0; i < (int)s.size(); i++) {
        TimeSlot& slot = data.slots[s[i].slotId];
        Subject&  subj = data.subjects[s[i].subjectId];

        // Build teacher name list
        std::string tnames;
        for (int j = 0; j < (int)s[i].teacherIds.size(); j++) {
            if (j > 0) tnames += " & ";
            tnames += data.teachers[s[i].teacherIds[j]].name;
        }

        f << slot.toString()          << ","
          << slot.day                 << ","
          << slot.period              << ","
          << periodLabel(slot.period) << ","
          << s[i].section             << ","
          << subj.code               << ","
          << subj.name               << ","
          << tnames                  << ","
          << data.rooms[s[i].roomId].name << ","
          << s[i].isLab              << "\n";
    }
    f.close();
    std::cout << "  Saved: " << fname << "\n";

    std::ofstream latest("output_latest_schedule.csv");
    latest << "Slot,Day,Period,Time,Section,SubjectCode,SubjectName,Teachers,Room,IsLab\n";
    for (int i = 0; i < (int)s.size(); i++) {
        TimeSlot& slot = data.slots[s[i].slotId];
        Subject&  subj = data.subjects[s[i].subjectId];

        std::string tnames;
        for (int j = 0; j < (int)s[i].teacherIds.size(); j++) {
            if (j > 0) tnames += " & ";
            tnames += data.teachers[s[i].teacherIds[j]].name;
        }

        latest << slot.toString()          << ","
               << slot.day                 << ","
               << slot.period              << ","
               << periodLabel(slot.period) << ","
               << s[i].section             << ","
               << subj.code                << ","
               << subj.name                << ","
               << tnames                   << ","
               << data.rooms[s[i].roomId].name << ","
               << s[i].isLab               << "\n";
    }
    latest.close();
    std::cout << "  Saved: output_latest_schedule.csv\n";

    // 2. One CSV per section
    std::vector<std::string> sections = getSections(data);
    for (int si = 0; si < (int)sections.size(); si++) {
        std::string sec   = sections[si];
        std::string sname = "output_v" + std::to_string(version)
                          + "_section_" + sec + ".csv";
        std::ofstream sf(sname);
        sf << "Day,Period,Time,SubjectCode,SubjectName,Teachers,Room\n";
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i].section != sec) continue;
            TimeSlot& slot = data.slots[s[i].slotId];
            Subject&  subj = data.subjects[s[i].subjectId];
            std::string tnames;
            for (int j = 0; j < (int)s[i].teacherIds.size(); j++) {
                if (j > 0) tnames += " & ";
                tnames += data.teachers[s[i].teacherIds[j]].name;
            }
            sf << slot.dayName()          << ","
               << slot.period             << ","
               << periodLabel(slot.period)<< ","
               << subj.code              << ","
               << subj.name              << ","
               << tnames                 << ","
               << data.rooms[s[i].roomId].name << "\n";
        }
        sf.close();
        std::cout << "  Saved: " << sname << "\n";

        std::string latestSectionName = "output_latest_section_" + sec + ".csv";
        std::ofstream latestSection(latestSectionName);
        latestSection << "Day,Period,Time,SubjectCode,SubjectName,Teachers,Room\n";
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i].section != sec) continue;
            TimeSlot& slot = data.slots[s[i].slotId];
            Subject&  subj = data.subjects[s[i].subjectId];
            std::string tnames;
            for (int j = 0; j < (int)s[i].teacherIds.size(); j++) {
                if (j > 0) tnames += " & ";
                tnames += data.teachers[s[i].teacherIds[j]].name;
            }
            latestSection << slot.dayName()           << ","
                          << slot.period              << ","
                          << periodLabel(slot.period) << ","
                          << subj.code                << ","
                          << subj.name                << ","
                          << tnames                   << ","
                          << data.rooms[s[i].roomId].name << "\n";
        }
        latestSection.close();
        std::cout << "  Saved: " << latestSectionName << "\n";
    }

    // 3. Teacher timetable CSV
    std::string tname = "output_v" + std::to_string(version) + "_teachers.csv";
    std::ofstream tf(tname);
    tf << "Teacher,Day,Period,Time,Section,SubjectCode,Room\n";
    for (int t = 0; t < (int)data.teachers.size(); t++) {
        for (int i = 0; i < (int)s.size(); i++) {
            bool found = false;
            for (int tid : s[i].teacherIds)
                if (tid == t) { found = true; break; }
            if (!found) continue;
            TimeSlot& slot = data.slots[s[i].slotId];
            tf << data.teachers[t].name              << ","
               << slot.dayName()                     << ","
               << slot.period                        << ","
               << periodLabel(slot.period)           << ","
               << s[i].section                       << ","
               << data.subjects[s[i].subjectId].code << ","
               << data.rooms[s[i].roomId].name       << "\n";
        }
    }
    tf.close();
    std::cout << "  Saved: " << tname << "\n";

    std::ofstream latestTeachers("output_latest_teachers.csv");
    latestTeachers << "Teacher,Day,Period,Time,Section,SubjectCode,Room\n";
    for (int t = 0; t < (int)data.teachers.size(); t++) {
        for (int i = 0; i < (int)s.size(); i++) {
            bool found = false;
            for (int tid : s[i].teacherIds)
                if (tid == t) { found = true; break; }
            if (!found) continue;
            TimeSlot& slot = data.slots[s[i].slotId];
            latestTeachers << data.teachers[t].name              << ","
                           << slot.dayName()                     << ","
                           << slot.period                        << ","
                           << periodLabel(slot.period)           << ","
                           << s[i].section                       << ","
                           << data.subjects[s[i].subjectId].code << ","
                           << data.rooms[s[i].roomId].name       << "\n";
        }
    }
    latestTeachers.close();
    std::cout << "  Saved: output_latest_teachers.csv\n";
}

} // namespace Output

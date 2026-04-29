#include "../include/checker.h"
#include <iostream>

namespace Checker {

int Result::hardCount() {
    int c = 0;
    for (int i = 0; i < (int)problems.size(); i++)
        if (problems[i].kind == "HARD") c++;
    return c;
}
int Result::softCount() {
    int c = 0;
    for (int i = 0; i < (int)problems.size(); i++)
        if (problems[i].kind == "SOFT") c++;
    return c;
}
int Result::score() { return hardCount()*100 + softCount()*10; }

Result checkAll(std::vector<ScheduleEntry>& s, InputData& data) {
    Result result;
    int n = (int)s.size();

    // HARD: Section has two classes at same time
    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            if (s[i].slotId == s[j].slotId && s[i].section == s[j].section) {
                Problem p;
                p.kind   = "HARD";
                p.detail = "Section " + s[i].section + " double-booked at "
                         + data.slots[s[i].slotId].toString();
                result.problems.push_back(p);
            }
        }
    }

    // HARD: Teacher in two places at same time
    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            if (s[i].slotId != s[j].slotId) continue;
            // Check if any teacher is shared
            for (int ta : s[i].teacherIds) {
                for (int tb : s[j].teacherIds) {
                    if (ta == tb) {
                        Problem p;
                        p.kind   = "HARD";
                        p.detail = "Teacher " + data.teachers[ta].name
                                 + " double-booked at "
                                 + data.slots[s[i].slotId].toString();
                        result.problems.push_back(p);
                    }
                }
            }
        }
    }

    // HARD: Room double-booked
    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            if (s[i].slotId == s[j].slotId && s[i].roomId == s[j].roomId) {
                Problem p;
                p.kind   = "HARD";
                p.detail = "Room " + data.rooms[s[i].roomId].name
                         + " double-booked at "
                         + data.slots[s[i].slotId].toString();
                result.problems.push_back(p);
            }
        }
    }

    // HARD: Lab subject not in lab room
    for (int i = 0; i < n; i++) {
        if (s[i].isLab && !data.rooms[s[i].roomId].isLab) {
            Problem p;
            p.kind   = "HARD";
            p.detail = data.subjects[s[i].subjectId].name
                     + " (Sec " + s[i].section + ") needs lab but got "
                     + data.rooms[s[i].roomId].name;
            result.problems.push_back(p);
        }
    }

    // SOFT: Teacher has 3+ back-to-back classes
    for (int t = 0; t < (int)data.teachers.size(); t++) {
        for (int day = 0; day < data.workingDays; day++) {
            int streak = 0;
            for (int p = 0; p < data.periodsPerDay; p++) {
                bool hasClass = false;
                for (int i = 0; i < n; i++) {
                    if (data.slots[s[i].slotId].day    != day)  continue;
                    if (data.slots[s[i].slotId].period != p)    continue;
                    for (int tid : s[i].teacherIds)
                        if (tid == t) { hasClass = true; break; }
                    if (hasClass) break;
                }
                streak = hasClass ? streak+1 : 0;
                if (streak >= 3) {
                    Problem prob;
                    prob.kind   = "SOFT";
                    prob.detail = data.teachers[t].name
                                + " has 3+ back-to-back on day "
                                + std::to_string(day);
                    result.problems.push_back(prob);
                    break;
                }
            }
        }
    }

    // SOFT: Class in last period
    int lastP = data.periodsPerDay - 1;
    for (int i = 0; i < n; i++) {
        if (data.slots[s[i].slotId].period == lastP) {
            Problem p;
            p.kind   = "SOFT";
            p.detail = data.subjects[s[i].subjectId].name
                     + " (Sec " + s[i].section + ") in last period";
            result.problems.push_back(p);
        }
    }

    return result;
}

void printResult(Result& r) {
    std::cout << "\n------------------------------------------\n";
    std::cout << "  Hard violations      : " << r.hardCount() << "\n";
    std::cout << "  Soft violations      : " << r.softCount() << "\n";
    std::cout << "  Score (lower=better) : " << r.score()     << "\n";
    if (r.problems.empty()) {
        std::cout << "  All constraints satisfied!\n";
    } else {
        // Only print first 10 to avoid flooding
        int show = r.problems.size() < 10 ? r.problems.size() : 10;
        for (int i = 0; i < show; i++)
            std::cout << "  [" << r.problems[i].kind << "] " << r.problems[i].detail << "\n";
        if ((int)r.problems.size() > 10)
            std::cout << "  ... and " << r.problems.size()-10 << " more\n";
    }
    std::cout << "------------------------------------------\n";
}

} // namespace Checker

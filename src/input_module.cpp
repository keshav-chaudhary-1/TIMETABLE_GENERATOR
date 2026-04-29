#include "../include/input_module.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace InputModule {

std::vector<std::string> splitLine(std::string line) {
    std::vector<std::string> parts;
    std::string part;
    std::stringstream ss(line);
    while (getline(ss, part, ',')) {
        while (!part.empty() && (part[0]==' '||part[0]=='\r')) part=part.substr(1);
        while (!part.empty() && (part.back()==' '||part.back()=='\r')) part.pop_back();
        parts.push_back(part);
    }
    return parts;
}

std::vector<int> splitIds(std::string s, char delim) {
    std::vector<int> ids;
    if (s.empty()) return ids;
    std::stringstream ss(s);
    std::string tok;
    while (getline(ss, tok, delim)) {
        while (!tok.empty() && tok[0]==' ') tok=tok.substr(1);
        while (!tok.empty() && tok.back()==' ') tok.pop_back();
        if (!tok.empty()) ids.push_back(stoi(tok));
    }
    return ids;
}

InputData loadFromCSV(std::string teacherFile,
                      std::string subjectFile,
                      std::string roomFile,
                      int workingDays,
                      int periodsPerDay) {
    InputData data;
    data.workingDays   = workingDays;
    data.periodsPerDay = periodsPerDay;
    std::string line;

    std::ifstream tf(teacherFile);
    if (!tf.is_open()) { std::cout<<"ERROR: Cannot open "<<teacherFile<<"\n"; exit(1); }
    getline(tf, line);
    while (getline(tf, line)) {
        if (line.empty()||line=="\r") continue;
        std::vector<std::string> p = splitLine(line);
        if (p.size() < 2) continue;
        Teacher t;
        t.id   = stoi(p[0]);
        t.name = p[1];
        data.teachers.push_back(t);
    }
    tf.close();

    std::ifstream sf(subjectFile);
    if (!sf.is_open()) { std::cout<<"ERROR: Cannot open "<<subjectFile<<"\n"; exit(1); }
    getline(sf, line);
    while (getline(sf, line)) {
        if (line.empty()||line=="\r") continue;
        std::vector<std::string> p = splitLine(line);
        if (p.size() < 7) continue;
        Subject s;
        s.id           = stoi(p[0]);
        s.code         = p[1];
        s.name         = p[2];
        s.hoursPerWeek = stoi(p[3]);
        s.requiresLab  = (stoi(p[4]) == 1);
        s.section      = p[5];
        s.teacherIds   = splitIds(p[6], '|');
        if (p.size() > 7) s.labTeacherIds = splitIds(p[7], '|');
        data.subjects.push_back(s);
    }
    sf.close();

    std::ifstream rf(roomFile);
    if (!rf.is_open()) 
    { 
        std::cout<<"ERROR: Cannot open "<<roomFile<<"\n"; exit(1); 
    }
    getline(rf, line);
    while (getline(rf, line)) 
    {
        if (line.empty()||line=="\r") continue;
        std::vector<std::string> p = splitLine(line);
        if (p.size() < 4) continue;
        Room r;
        r.id       = stoi(p[0]);
        r.name     = p[1];
        r.isLab    = (stoi(p[2]) == 1);
        r.capacity = stoi(p[3]);
        data.rooms.push_back(r);
    }
    rf.close();

    int id = 0;
    for (int d = 0; d < workingDays; d++) 
    {
        for (int p = 0; p < periodsPerDay; p++) {
            TimeSlot slot;
            slot.id     = id++;
            slot.day    = d;
            slot.period = p;
            data.slots.push_back(slot);
        }
    }

    return data;
}

void printSummary(InputData& data) {
    // Count unique sections
    std::vector<std::string> sections;
    for (int i = 0; i < (int)data.subjects.size(); i++) {
        std::string sec = data.subjects[i].section;
        bool found = false;
        for (int j = 0; j < (int)sections.size(); j++)
            if (sections[j] == sec) { found = true; break; }
        if (!found) sections.push_back(sec);
    }

    std::cout << "\n==========================================\n";
    std::cout << "   CSE 2nd Year Timetable Generator\n";
    std::cout << "       INPUT DATA SUMMARY\n";
    std::cout << "==========================================\n";
    std::cout << "  Teachers   : " << data.teachers.size() << "\n";
    std::cout << "  Subjects   : " << data.subjects.size() << "\n";
    std::cout << "  Sections   : " << sections.size() << " (";
    for (int i = 0; i < (int)sections.size(); i++) {
        std::cout << sections[i];
        if (i+1 < (int)sections.size()) std::cout << ", ";
    }
    std::cout << ")\n";
    std::cout << "  Rooms      : " << data.rooms.size()    << "\n";
    std::cout << "  Time Slots : " << data.slots.size()
              << " (" << data.workingDays << " days x "
              << data.periodsPerDay << " periods)\n";
    std::cout << "==========================================\n\n";
}

} 

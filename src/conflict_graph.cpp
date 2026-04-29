#include "../include/conflict_graph.h"
#include <iostream>

namespace ConflictGraph {

Graph build(InputData& data) {
    Graph g;

    // One node per required lecture hour
    int nodeId = 0;
    for (int i = 0; i < (int)data.subjects.size(); i++) {
        Subject& subj = data.subjects[i];
        // Use lab teachers if it's a lab, else theory teachers
        std::vector<int> teachers = subj.requiresLab ? subj.labTeacherIds : subj.teacherIds;
        if (teachers.empty()) teachers = subj.teacherIds;

        for (int h = 0; h < subj.hoursPerWeek; h++) {
            LectureNode node;
            node.nodeId      = nodeId++;
            node.subjectId   = subj.id;
            node.section     = subj.section;
            node.requiresLab = subj.requiresLab;
            node.teacherIds  = teachers;
            g.nodes.push_back(node);
        }
    }

    int n = (int)g.nodes.size();
    g.hasConflict.resize(n, std::vector<bool>(n, false));

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            LectureNode& a = g.nodes[i];
            LectureNode& b = g.nodes[j];
            bool clash = false;

            // Same section — students can't be in two places
            if (a.section == b.section) clash = true;

            // Same subject — spread it across the week
            if (a.subjectId == b.subjectId) clash = true;

            // Shared teacher — teacher can't be in two places
            // Check if any teacher appears in both nodes
            if (!clash) {
                for (int ta : a.teacherIds) {
                    for (int tb : b.teacherIds) {
                        if (ta == tb) { clash = true; break; }
                    }
                    if (clash) break;
                }
            }

            if (clash) {
                g.hasConflict[i][j] = true;
                g.hasConflict[j][i] = true;
            }
        }
    }

    return g;
}

void printInfo(Graph& g) {
    int edges = 0;
    int n = (int)g.nodes.size();
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (g.hasConflict[i][j]) edges++;
    std::cout << "  Lecture nodes : " << n     << "\n";
    std::cout << "  Conflict edges: " << edges << "\n";
}

} // namespace ConflictGraph

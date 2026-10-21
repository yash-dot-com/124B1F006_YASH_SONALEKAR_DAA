
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Allocation {
    int subject;
    int slot;
    int room;
};

int main() {
    vector<string> subjects = {
        "DAA", "AIML", "SPM", "EAC", "MDM-3"
    };

    vector<string> colors = {
        "RED", "GREEN", "BLUE", "YELLOW",
        "PURPLE", "ORANGE", "CYAN"
    };

    int n = subjects.size();
    int m;

    vector<vector<int>> graph(n);
    vector<pair<int, int>> conflicts;

    cout << "\n========== UNIVERSITY EXAM TIMETABLE ==========\n";

    // Display subjects
    cout << "\n--------------- SUBJECT LIST ---------------\n";
    cout << left << setw(12) << "Node"
         << setw(15) << "Subject" << "\n";
    cout << string(27, '-') << "\n";

    for (int i = 0; i < n; i++) {
        cout << left << setw(12) << i
             << setw(15) << subjects[i] << "\n";
    }

    cout << string(27, '-') << "\n";

    // Input conflicts
    cout << "\nEnter number of conflicts: ";
    cin >> m;

    if (!cin || m < 0) {
        cout << "Invalid conflict count.\n";
        return 1;
    }

    cout << "Enter conflicting pairs (u v):\n";

    for (int i = 0; i < m; i++) {
        int u, v;
        cin >> u >> v;

        if (!cin || u < 0 || u >= n ||
            v < 0 || v >= n || u == v) {
            cout << "Invalid conflict!\n";
            return 1;
        }

        graph[u].push_back(v);
        graph[v].push_back(u);
        conflicts.push_back({u, v});
    }

    // Display conflict table
    cout << "\n================ CONFLICT GRAPH ================\n";

    cout << left
         << setw(12) << "Conflict"
         << setw(18) << "Subject 1"
         << setw(18) << "Subject 2"
         << "Restriction\n";

    cout << string(65, '-') << "\n";

    for (int i = 0; i < m; i++) {
        int u = conflicts[i].first;
        int v = conflicts[i].second;

        cout << left
             << setw(12) << i + 1
             << setw(18) << subjects[u]
             << setw(18) << subjects[v]
             << "Different slots\n";
    }

    cout << string(65, '-') << "\n";

    // Greedy graph colouring
    vector<int> slot(n, -1);

    for (int u = 0; u < n; u++) {
        vector<bool> used(n, false);

        for (int v : graph[u]) {
            if (slot[v] != -1) {
                used[slot[v]] = true;
            }
        }

        int c = 0;

        while (used[c]) {
            c++;
        }

        slot[u] = c;
    }

    int totalSlots = *max_element(slot.begin(), slot.end()) + 1;

    // Colour assignment table
    cout << "\n================ GRAPH COLOURING =================\n";

    cout << left
         << setw(12) << "Node"
         << setw(15) << "Subject"
         << setw(12) << "Color ID"
         << setw(15) << "Color"
         << setw(12) << "Time Slot"
         << "\n";

    cout << string(66, '-') << "\n";

    for (int i = 0; i < n; i++) {
        string colorName;

        if (slot[i] < (int)colors.size())
            colorName = colors[slot[i]];
        else
            colorName = "COLOR-" + to_string(slot[i] + 1);

        cout << left
             << setw(12) << i
             << setw(15) << subjects[i]
             << setw(12) << slot[i] + 1
             << setw(15) << colorName
             << setw(12) << slot[i] + 1
             << "\n";
    }

    cout << string(66, '-') << "\n";

    // Room input
    int roomCount;

    cout << "\nEnter number of classrooms: ";
    cin >> roomCount;

    if (!cin || roomCount <= 0) {
        cout << "Invalid number of rooms.\n";
        return 1;
    }

    vector<int> capacity(roomCount);

    cout << "Enter capacity of each room:\n";

    for (int r = 0; r < roomCount; r++) {
        cout << "Room " << r + 1 << " capacity: ";
        cin >> capacity[r];

        if (!cin || capacity[r] <= 0) {
            cout << "Invalid room capacity.\n";
            return 1;
        }
    }

    vector<int> students(n);

    cout << "\nEnter student count for each subject:\n";

    for (int i = 0; i < n; i++) {
        cout << subjects[i] << ": ";
        cin >> students[i];

        if (!cin || students[i] <= 0) {
            cout << "Invalid student count.\n";
            return 1;
        }
    }

    // Classroom capacity table
    cout << "\n================ CLASSROOM DETAILS ================\n";

    cout << left
         << setw(15) << "Room"
         << setw(20) << "Capacity"
         << "\n";

    cout << string(35, '-') << "\n";

    for (int r = 0; r < roomCount; r++) {
        cout << left
             << setw(15) << r + 1
             << setw(20) << capacity[r]
             << "\n";
    }

    cout << string(35, '-') << "\n";

    // Allocate classrooms
    vector<Allocation> allocations;

    for (int t = 0; t < totalSlots; t++) {

        // Rooms are free again in a new time slot
        vector<bool> occupied(roomCount, false);

        for (int i = 0; i < n; i++) {

            if (slot[i] != t)
                continue;

            int chosen = -1;

            // Choose smallest available room that fits
            for (int r = 0; r < roomCount; r++) {

                if (!occupied[r] &&
                    capacity[r] >= students[i] &&
                    (chosen == -1 ||
                     capacity[r] < capacity[chosen])) {

                    chosen = r;
                }
            }

            if (chosen != -1)
                occupied[chosen] = true;

            allocations.push_back({i, t, chosen});
        }
    }

    // Final examination timetable
    cout << "\n================ FINAL EXAM TIMETABLE ================\n";

    cout << left
         << setw(12) << "Time Slot"
         << setw(15) << "Color"
         << setw(15) << "Subject"
         << setw(12) << "Students"
         << setw(12) << "Room"
         << setw(15) << "Room Capacity"
         << "Status\n";

    cout << string(97, '-') << "\n";

    int allocatedCount = 0;

    for (auto &a : allocations) {

        string colorName;

        if (a.slot < (int)colors.size())
            colorName = colors[a.slot];
        else
            colorName = "COLOR-" + to_string(a.slot + 1);

        cout << left
             << setw(12) << a.slot + 1
             << setw(15) << colorName
             << setw(15) << subjects[a.subject]
             << setw(12) << students[a.subject];

        if (a.room == -1) {
            cout << setw(12) << "-"
                 << setw(15) << "-"
                 << "NO ROOM\n";
        }
        else {
            cout << setw(12) << a.room + 1
                 << setw(15) << capacity[a.room]
                 << "ALLOCATED\n";

            allocatedCount++;
        }
    }

    cout << string(97, '-') << "\n";

    // Verify conflict-free slot assignment
    bool conflictFree = true;

    for (auto &p : conflicts) {
        if (slot[p.first] == slot[p.second])
            conflictFree = false;
    }

    // Final summary
    cout << "\n================ SCHEDULING SUMMARY ================\n";

    cout << "Total Subjects       : " << n << "\n";
    cout << "Total Conflicts      : " << m << "\n";
    cout << "Total Time Slots     : " << totalSlots << "\n";
    cout << "Colors Used          : " << totalSlots << "\n";
    cout << "Available Classrooms : " << roomCount << "\n";
    cout << "Exams Allocated      : " << allocatedCount << "/" << n << "\n";

    cout << "Conflict Check       : "
         << (conflictFree ? "PASSED" : "FAILED") << "\n";

    cout << "Room Allocation      : "
         << (allocatedCount == n ? "COMPLETE" : "INCOMPLETE")
         << "\n";

    cout << "====================================================\n";

    return 0;
}

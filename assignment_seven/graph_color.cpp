#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
int main() {
   vector<string> subjects = {"DAA", "AIML", "SPM", "EAC", "MDM-3"};
   int n = subjects.size();
   int m;
   vector<vector<int>> graph(n);
   cout << "Subjects:\n";
   for (int i = 0; i < n; i++) {
       cout << i << " - " << subjects[i] << "\n";
   }
   cout << "Enter number of conflicts: ";
   cin >> m;
   cout << "Enter conflicting pairs (u v):\n";
   for (int i = 0; i < m; i++) {
       int u, v;
       cin >> u >> v;
       if (u < 0 || u >= n || v < 0 || v >= n || u == v) {
           cout << "Invalid conflict!\n";
           return 1;
       }
       graph[u].push_back(v);
       graph[v].push_back(u);
   }
   // Greedy colouring: adjacent exams cannot use the same slot.
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
   int roomCount;
   cout << "Enter number of rooms: ";
   cin >> roomCount;
   vector<int> capacity(roomCount);
   cout << "Enter capacity of each room:\n";
   for (int r = 0; r < roomCount; r++) {
       cin >> capacity[r];
   }
   vector<int> students(n);
   cout << "Enter student count for DAA, AIML, SPM, EAC, MDM-3:\n";
   for (int i = 0; i < n; i++) {
       cin >> students[i];
   }
   int totalSlots = *max_element(slot.begin(), slot.end()) + 1;
   cout << "\n----- EXAM TIMETABLE -----\n";
   for (int t = 0; t < totalSlots; t++) {
       cout << "\nSlot " << t + 1 << ":\n";
       vector<bool> occupied(roomCount, false);
       for (int i = 0; i < n; i++) {
           if (slot[i] != t) {
               continue;
           }
           int chosen = -1;
           // Choose the smallest currently unused room that fits.
           for (int r = 0; r < roomCount; r++) {
               if (!occupied[r] && capacity[r] >= students[i] &&
                   (chosen == -1 || capacity[r] < capacity[chosen])) {
                   chosen = r;
               }
           }
           cout << subjects[i] << " (" << students[i] << " students) - ";
           if (chosen == -1) {
               cout << "NO SUITABLE ROOM\n";
           } else {
               occupied[chosen] = true;
               cout << "Room " << chosen + 1 << "\n";
           }
       }
   }
   cout << "\nTotal exam slots: " << totalSlots << "\n";
   return 0;
}

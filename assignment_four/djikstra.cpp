
#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <iomanip>
#include <algorithm>
#include <string>
using namespace std;

// Find shortest distances from the ambulance location
void dijkstra(
    int source,
    vector<vector<pair<int,int>>> &graph,
    vector<int> &dist,
    vector<int> &parent
) {
    int V = graph.size();

    dist.assign(V, INT_MAX);
    parent.assign(V, -1);

    dist[source] = 0;

    priority_queue<
        pair<int,int>,
        vector<pair<int,int>>,
        greater<pair<int,int>>
    > pq;

    pq.push({0, source});

    while (!pq.empty()) {

        int u = pq.top().second;
        int d = pq.top().first;
        pq.pop();

        if (d > dist[u])
            continue;

        for (auto &edge : graph[u]) {

            int v = edge.first;
            int w = edge.second;

            // Update when a shorter route is found
            if (dist[v] > dist[u] + w) {

                dist[v] = dist[u] + w;

                // Store the previous node in the route
                parent[v] = u;

                pq.push({dist[v], v});
            }
        }
    }
}

// Reconstruct shortest path using parent array
string getPath(int source, int destination,
               vector<int> &parent) {

    vector<int> path;

    int current = destination;

    while (current != -1) {
        path.push_back(current);
        current = parent[current];
    }

    reverse(path.begin(), path.end());

    if (path.empty() || path[0] != source)
        return "No path";

    string result = "";

    for (int i = 0; i < (int)path.size(); i++) {
        result += to_string(path[i]);

        if (i != (int)path.size() - 1)
            result += " -> ";
    }

    return result;
}

int main() {

    int V, E;

    cout << "\n========== AMBULANCE ROUTE OPTIMIZATION ==========\n";

    cout << "Enter number of intersections: ";
    cin >> V;

    cout << "Enter number of roads: ";
    cin >> E;

    if (!cin || V <= 0 || E < 0) {
        cout << "Invalid number of intersections or roads.\n";
        return 1;
    }

    vector<vector<pair<int,int>>> graph(V);

    struct Road {
        int from;
        int to;
        int time;
    };

    vector<Road> roads;

    cout << "\nEnter roads as: from to travel_time\n";

    for (int i = 0; i < E; i++) {

        int u, v, w;
        cin >> u >> v >> w;

        if (!cin || u < 0 || u >= V ||
            v < 0 || v >= V || w < 0) {

            cout << "Invalid road data.\n";
            return 1;
        }

        // Undirected road network
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});

        roads.push_back({u, v, w});
    }

    cout << "\n================ ROAD NETWORK ================\n";

    cout << left
         << setw(12) << "Road No."
         << setw(15) << "From"
         << setw(15) << "To"
         << setw(18) << "Travel Time"
         << "\n";

    cout << string(60, '-') << "\n";

    for (int i = 0; i < E; i++) {

        cout << left
             << setw(12) << i + 1
             << setw(15) << roads[i].from
             << setw(15) << roads[i].to
             << setw(18) << roads[i].time
             << "\n";
    }

    cout << string(60, '-') << "\n";

    int source;

    cout << "\nEnter ambulance start location: ";
    cin >> source;

    if (!cin || source < 0 || source >= V) {
        cout << "Invalid source node.\n";
        return 1;
    }

    int H;

    cout << "Enter number of hospitals: ";
    cin >> H;

    if (!cin || H <= 0) {
        cout << "Invalid number of hospitals.\n";
        return 1;
    }

    vector<int> hospitals(H);

    cout << "Enter hospital node IDs: ";

    for (int i = 0; i < H; i++) {
        cin >> hospitals[i];

        if (!cin || hospitals[i] < 0 ||
            hospitals[i] >= V) {
            cout << "Invalid hospital node.\n";
            return 1;
        }
    }

    vector<int> dist;
    vector<int> parent;

    dijkstra(source, graph, dist, parent);

    // Print shortest travel time to every intersection
    cout << "\n=============== SHORTEST DISTANCES ===============\n";

    cout << left
         << setw(15) << "Node"
         << setw(22) << "Travel Time (min)"
         << setw(15) << "Previous Node"
         << "\n";

    cout << string(55, '-') << "\n";

    for (int i = 0; i < V; i++) {

        cout << left << setw(15) << i;

        if (dist[i] == INT_MAX) {
            cout << setw(22) << "Unreachable"
                 << setw(15) << "-"
                 << "\n";
        }
        else {
            cout << setw(22) << dist[i];

            if (parent[i] == -1)
                cout << setw(15) << "-";
            else
                cout << setw(15) << parent[i];

            cout << "\n";
        }
    }

    cout << string(55, '-') << "\n";

    // Compare all hospital locations
    cout << "\n================ HOSPITAL COMPARISON ================\n";

    cout << left
         << setw(15) << "Hospital"
         << setw(20) << "Travel Time"
         << setw(18) << "Status"
         << "Shortest Route"
         << "\n";

    cout << string(90, '-') << "\n";

    int minTime = INT_MAX;
    int nearestHospital = -1;

    for (int h : hospitals) {

        cout << left << setw(15) << h;

        if (dist[h] == INT_MAX) {

            cout << setw(20) << "-"
                 << setw(18) << "Unreachable"
                 << "No path"
                 << "\n";
        }
        else {

            cout << setw(20) << dist[h]
                 << setw(18) << "Reachable"
                 << getPath(source, h, parent)
                 << "\n";

            if (dist[h] < minTime) {
                minTime = dist[h];
                nearestHospital = h;
            }
        }
    }

    cout << string(90, '-') << "\n";

    // Final result
    cout << "\n================== FINAL RESULT ==================\n";

    cout << "Ambulance Start Node : " << source << "\n";

    if (nearestHospital == -1) {
        cout << "No hospital is reachable.\n";
    }
    else {
        cout << "Nearest Hospital     : Node "
             << nearestHospital << "\n";

        cout << "Shortest Path        : "
             << getPath(source, nearestHospital, parent)
             << "\n";

        cout << "Minimum Travel Time  : "
             << minTime << " minutes\n";
    }

    cout << "==================================================\n";

    return 0;
}

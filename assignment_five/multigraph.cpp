#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <cmath>
#include <string>
#include <limits>
using namespace std;

const double INF = 1e30;

struct Edge {
    int to;
    double base_cost;
    double cur_cost;
};

int stageOf(int node, const vector<int>& start, const vector<int>& count) {
    for (int s = 0; s < (int)count.size(); ++s)
        if (node >= start[s] && node < start[s] + count[s]) return s;
    return -1;
}

void recompute(int u, const vector<vector<Edge>>& adj,
               vector<double>& best_cost, vector<int>& next_node) {
    double best = INF;
    int next = -1;
    for (const Edge& e : adj[u]) {
        if (best_cost[e.to] >= INF / 2) continue;
        double candidate = e.cur_cost + best_cost[e.to];
        if (candidate < best) {
            best = candidate;
            next = e.to;
        }
    }
    best_cost[u] = best;
    next_node[u] = next;
}

void printEdges(const vector<vector<Edge>>& adj,
                const vector<int>& start, const vector<int>& count) {
    cout << "\n======================= GRAPH EDGES =======================\n";
    cout << left << setw(10) << "From" << setw(10) << "To"
         << setw(12) << "Stage" << right << setw(14) << "Base Cost"
         << setw(16) << "Current Cost" << '\n';
    cout << string(62, '-') << '\n';
    for (int u = 0; u < (int)adj.size(); ++u) {
        for (const Edge& e : adj[u]) {
            string transition = to_string(stageOf(u, start, count) + 1) + " -> " +
                                to_string(stageOf(e.to, start, count) + 1);
            cout << left << setw(10) << u << setw(10) << e.to
                 << setw(12) << transition << right << setw(14) << e.base_cost
                 << setw(16) << e.cur_cost << '\n';
        }
    }
    cout << string(62, '-') << '\n';
}

void printDP(const vector<double>& best_cost, const vector<int>& next_node,
             const vector<int>& start, const vector<int>& count) {
    cout << "\n================== MINIMUM COST DP TABLE ==================\n";
    cout << left << setw(12) << "Stage" << setw(12) << "Node"
         << setw(23) << "Min Cost to Finish" << "Next Node\n";
    cout << string(62, '-') << '\n';
    for (int s = 0; s < (int)count.size(); ++s) {
        for (int k = 0; k < count[s]; ++k) {
            int u = start[s] + k;
            cout << left << setw(12) << s + 1 << setw(12) << u;
            if (best_cost[u] >= INF / 2)
                cout << setw(23) << "Unreachable";
            else
                cout << setw(23) << best_cost[u];
            if (next_node[u] == -1) cout << "-\n";
            else cout << next_node[u] << '\n';
        }
    }
    cout << string(62, '-') << '\n';
}

void printRoute(int src, const vector<vector<Edge>>& adj,
                const vector<double>& best_cost, const vector<int>& next_node) {
    cout << "\n===================== OPTIMAL ROUTE ======================\n";
    if (best_cost[src] >= INF / 2) {
        cout << "No valid route from node " << src << " to the last stage.\n";
        return;
    }
    cout << left << setw(12) << "From" << setw(12) << "To"
         << right << setw(16) << "Edge Cost" << setw(20) << "Cumulative Cost\n";
    cout << string(60, '-') << '\n';

    int u = src;
    int steps = 0;
    double total = 0;
    string path = to_string(src);
    while (next_node[u] != -1 && steps < (int)adj.size()) {
        int v = next_node[u];
        bool found = false;
        double edgeCost = 0;
        for (const Edge& e : adj[u]) {
            if (e.to == v) {
                edgeCost = e.cur_cost;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Route edge missing.\n";
            return;
        }
        total += edgeCost;
        cout << left << setw(12) << u << setw(12) << v
             << right << setw(16) << edgeCost << setw(20) << total << '\n';
        path += " -> " + to_string(v);
        u = v;
        ++steps;
    }
    cout << string(60, '-') << '\n';
    cout << "Shortest Path     : " << path << '\n';
    cout << "Minimum Total Cost: " << total << '\n';
}

int main() {
    cout << fixed << setprecision(2);
    cout << "========== MULTISTAGE GRAPH - DYNAMIC PROGRAMMING ==========\n";

    int S;
    cout << "Enter number of stages: ";
    cin >> S;
    if (!cin || S < 2) { cout << "Invalid number of stages.\n"; return 1; }

    vector<int> stage_count(S), stage_start(S);
    int N = 0;
    cout << "Enter number of nodes in each stage (" << S << " values): ";
    for (int s = 0; s < S; ++s) {
        cin >> stage_count[s];
        if (!cin || stage_count[s] <= 0) { cout << "Each stage must have at least one node.\n"; return 1; }
        stage_start[s] = N;
        N += stage_count[s];
    }

    int M;
    cout << "Enter number of directed edges: ";
    cin >> M;
    if (!cin || M < 0) { cout << "Invalid edge count.\n"; return 1; }

    vector<vector<Edge>> adj(N);
    vector<vector<int>> rev_adj(N);
    cout << "Enter edges as: from to cost\n";
    for (int i = 0; i < M; ++i) {
        int u, v;
        double cost;
        cin >> u >> v >> cost;
        if (!cin || u < 0 || u >= N || v < 0 || v >= N ||
            !isfinite(cost) || cost < 0 ||
            stageOf(u, stage_start, stage_count) >= stageOf(v, stage_start, stage_count)) {
            cout << "Invalid edge. Edges must go from an earlier stage to a later stage.\n";
            return 1;
        }
        for (const Edge& e : adj[u]) {
            if (e.to == v) { cout << "Duplicate directed edge is not allowed.\n"; return 1; }
        }
        adj[u].push_back({v, cost, cost});
        rev_adj[v].push_back(u);
    }

    vector<double> best_cost(N, INF);
    vector<int> next_node(N, -1);
    for (int k = 0; k < stage_count[S - 1]; ++k)
        best_cost[stage_start[S - 1] + k] = 0;

    for (int s = S - 2; s >= 0; --s) {
        for (int k = 0; k < stage_count[s]; ++k) {
            int u = stage_start[s] + k;
            recompute(u, adj, best_cost, next_node);
        }
    }

    printEdges(adj, stage_start, stage_count);
    printDP(best_cost, next_node, stage_start, stage_count);

    cout << "\n============== STAGE 1 SOURCE COSTS ==============\n";
    for (int k = 0; k < stage_count[0]; ++k) {
        int u = stage_start[0] + k;
        cout << "Node " << u << " : ";
        if (best_cost[u] >= INF / 2) cout << "Unreachable\n";
        else cout << best_cost[u] << '\n';
    }

    int src;
    cout << "\nEnter source node (from first stage): ";
    cin >> src;
    if (!cin || src < stage_start[0] || src >= stage_start[0] + stage_count[0]) {
        cout << "Invalid source node.\n";
        return 1;
    }
    printRoute(src, adj, best_cost, next_node);

    int Q;
    cout << "\nEnter number of live edge-cost updates (0 to skip): ";
    cin >> Q;
    if (!cin || Q < 0) { cout << "Invalid update count.\n"; return 1; }
    for (int z = 1; z <= Q; ++z) {
        int u, v;
        double multiplier;
        cout << "\nUpdate " << z << " - enter: from to multiplier: ";
        cin >> u >> v >> multiplier;
        if (!cin || u < 0 || u >= N || v < 0 || v >= N ||
            !isfinite(multiplier) || multiplier < 0) {
            cout << "Invalid update.\n";
            return 1;
        }
        bool found = false;
        double oldCost = 0, newCost = 0;
        for (Edge& e : adj[u]) {
            if (e.to == v) {
                found = true;
                oldCost = e.cur_cost;
                e.cur_cost = e.base_cost * multiplier;
                newCost = e.cur_cost;
                break;
            }
        }
        if (!found) {
            cout << "Edge " << u << " -> " << v << " does not exist. Skipped.\n";
            continue;
        }
        cout << "\n=================== EDGE UPDATE " << z << " ===================\n";
        cout << left << setw(12) << "From" << setw(12) << "To"
             << right << setw(14) << "Old Cost" << setw(14) << "Multiplier"
             << setw(14) << "New Cost" << '\n';
        cout << string(66, '-') << '\n';
        cout << left << setw(12) << u << setw(12) << v << right
             << setw(14) << oldCost << setw(14) << multiplier
             << setw(14) << newCost << '\n';
        cout << string(66, '-') << '\n';

        queue<int> q;
        double oldBest = best_cost[u];
        recompute(u, adj, best_cost, next_node);
        if (fabs(best_cost[u] - oldBest) > 1e-9) q.push(u);
        while (!q.empty()) {
            int node = q.front();
            q.pop();
            for (int pred : rev_adj[node]) {
                double previous = best_cost[pred];
                recompute(pred, adj, best_cost, next_node);
                if (fabs(best_cost[pred] - previous) > 1e-9)
                    q.push(pred);
            }
        }
        printDP(best_cost, next_node, stage_start, stage_count);
        printRoute(src, adj, best_cost, next_node);
    }

    cout << "\n======================= FINAL SUMMARY =======================\n";
    cout << "Total Nodes   : " << N << '\n';
    cout << "Total Edges   : " << M << '\n';
    cout << "Total Stages  : " << S << '\n';
    cout << "Source Node   : " << src << '\n';
    cout << "Updates Asked : " << Q << '\n';
    if (best_cost[src] >= INF / 2) cout << "Final Min Cost: Unreachable\n";
    else cout << "Final Min Cost: " << best_cost[src] << '\n';
    return 0;
}

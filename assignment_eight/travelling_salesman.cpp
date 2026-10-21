
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <string>
using namespace std;

const int INF = 1000000000;

struct Node {
    vector<vector<int>> matrix;
    vector<int> path;
    int bound;
};

struct Compare {
    bool operator()(const Node &a, const Node &b) const {
        return a.bound > b.bound;
    }
};

// Reduce rows and columns to calculate the lower bound
int reduceMatrix(vector<vector<int>> &matrix) {
    int n = matrix.size();
    int reduction = 0;

    // Row reduction
    for (int i = 0; i < n; i++) {
        int minimum = INF;

        for (int j = 0; j < n; j++) {
            minimum = min(minimum, matrix[i][j]);
        }

        if (minimum == INF)
            continue;

        reduction += minimum;

        for (int j = 0; j < n; j++) {
            if (matrix[i][j] != INF)
                matrix[i][j] -= minimum;
        }
    }

    // Column reduction
    for (int j = 0; j < n; j++) {
        int minimum = INF;

        for (int i = 0; i < n; i++) {
            minimum = min(minimum, matrix[i][j]);
        }

        if (minimum == INF)
            continue;

        reduction += minimum;

        for (int i = 0; i < n; i++) {
            if (matrix[i][j] != INF)
                matrix[i][j] -= minimum;
        }
    }

    return reduction;
}

// Print cost matrix in tabular format
void printMatrix(const vector<vector<int>> &matrix) {
    int n = matrix.size();

    cout << left << setw(12) << "From/To";

    for (int j = 0; j < n; j++) {
        cout << setw(12) << ("City " + to_string(j));
    }

    cout << "\n" << string(12 * (n + 1), '-') << "\n";

    for (int i = 0; i < n; i++) {
        cout << left << setw(12) << ("City " + to_string(i));

        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == INF)
                cout << setw(12) << "INF";
            else
                cout << setw(12) << matrix[i][j];
        }

        cout << "\n";
    }

    cout << string(12 * (n + 1), '-') << "\n";
}

// Convert route into readable form
string getRoute(const vector<int> &path) {
    string route = "";

    for (int city : path) {
        route += to_string(city) + " -> ";
    }

    route += "0";
    return route;
}

int main() {
    int n;

    cout << "\n========== TRAVELLING SALESMAN PROBLEM ==========\n";
    cout << "Algorithm: Least Cost Branch and Bound\n\n";

    cout << "Enter number of cities: ";
    cin >> n;

    if (!cin || n < 2) {
        cout << "Invalid number of cities.\n";
        return 1;
    }

    vector<vector<int>> cost(n, vector<int>(n));

    cout << "\nEnter cost matrix (0 for no road):\n";

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int value;
            cin >> value;

            if (!cin || value < 0) {
                cout << "Invalid travel cost.\n";
                return 1;
            }

            if (i == j || value == 0)
                cost[i][j] = INF;
            else
                cost[i][j] = value;
        }
    }

    // Display original cost matrix
    cout << "\n================ ORIGINAL COST MATRIX ================\n";
    printMatrix(cost);

    // Initialize root node
    Node root;
    root.matrix = cost;
    root.path.push_back(0);

    root.bound = reduceMatrix(root.matrix);

    int initialBound = root.bound;

    cout << "\n================ INITIAL REDUCED MATRIX ================\n";
    printMatrix(root.matrix);

    cout << "Initial Lower Bound : " << initialBound << "\n";

    // Priority queue: smallest bound first
    priority_queue<Node, vector<Node>, Compare> pq;
    pq.push(root);

    int best = INF;
    vector<int> answer;

    int expandedNodes = 0;
    int generatedNodes = 1;  // Includes root
    int prunedNodes = 0;
    int completeTours = 0;

    // Branch and Bound algorithm
    while (!pq.empty()) {

        Node current = pq.top();
        pq.pop();

        // Skip if this route cannot improve best answer
        if (current.bound >= best) {
            prunedNodes++;
            continue;
        }

        expandedNodes++;

        int u = current.path.back();

        // Check if all cities have been visited
        if ((int)current.path.size() == n) {

            if (cost[u][0] == INF)
                continue;

            int total = cost[u][0];

            // Calculate actual route cost
            for (int k = 1; k < n; k++) {
                total += cost[current.path[k - 1]]
                             [current.path[k]];
            }

            completeTours++;

            if (total < best) {
                best = total;
                answer = current.path;
            }

            continue;
        }

        // Generate possible next cities
        for (int v = 1; v < n; v++) {

            bool visited = false;

            for (int city : current.path) {
                if (city == v) {
                    visited = true;
                    break;
                }
            }

            if (visited || current.matrix[u][v] == INF)
                continue;

            Node child = current;

            child.path.push_back(v);

            // Add the reduced edge cost
            child.bound += current.matrix[u][v];

            // Block outgoing roads from current city
            for (int j = 0; j < n; j++) {
                child.matrix[u][j] = INF;
            }

            // Block incoming roads to next city
            for (int i = 0; i < n; i++) {
                child.matrix[i][v] = INF;
            }

            // Prevent returning to source too early
            if ((int)child.path.size() < n) {
                child.matrix[v][0] = INF;
            }

            // Calculate updated lower bound
            child.bound += reduceMatrix(child.matrix);

            generatedNodes++;

            if (child.bound < best) {
                pq.push(child);
            }
            else {
                prunedNodes++;
            }
        }
    }

    if (best == INF) {
        cout << "\nNo possible complete tour exists.\n";
        return 0;
    }

    // Display selected route
    cout << "\n================ OPTIMAL ROUTE =================\n";

    cout << left
         << setw(10) << "Step"
         << setw(12) << "From"
         << setw(12) << "To"
         << setw(16) << "Travel Cost"
         << setw(20) << "Cumulative Cost"
         << "\n";

    cout << string(70, '-') << "\n";

    int cumulativeCost = 0;

    for (int i = 0; i < n; i++) {

        int from = answer[i];
        int to;

        if (i == n - 1)
            to = 0;
        else
            to = answer[i + 1];

        int edgeCost = cost[from][to];
        cumulativeCost += edgeCost;

        cout << left
             << setw(10) << i + 1
             << setw(12) << from
             << setw(12) << to
             << setw(16) << edgeCost
             << setw(20) << cumulativeCost
             << "\n";
    }

    cout << string(70, '-') << "\n";

    // Print final result
    cout << "\n================ FINAL RESULT =================\n";

    cout << "Starting City       : 0\n";
    cout << "Total Cities        : " << n << "\n";
    cout << "Cities Visited      : " << answer.size() << "\n";
    cout << "Shortest Tour       : " << getRoute(answer) << "\n";
    cout << "Minimum Travel Cost : " << best << "\n";

    cout << "\n============== ALGORITHM STATISTICS ==============\n";

    cout << "Initial Lower Bound : " << initialBound << "\n";
    cout << "Nodes Generated     : " << generatedNodes << "\n";
    cout << "Nodes Expanded      : " << expandedNodes << "\n";
    cout << "Branches Pruned     : " << prunedNodes << "\n";
    cout << "Complete Tours Found: " << completeTours << "\n";

    cout << "\nAlgorithm           : Least Cost Branch and Bound\n";
    cout << "==================================================\n";

    return 0;
}

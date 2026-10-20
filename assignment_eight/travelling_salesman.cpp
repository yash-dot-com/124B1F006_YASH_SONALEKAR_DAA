#include <iostream>

#include <vector>

#include <queue>

#include <algorithm>

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

int reduceMatrix(vector<vector<int>> &matrix) {

    int n = matrix.size();

    int reduction = 0;

    // Subtract the smallest finite value from every row.

    for (int i = 0; i < n; i++) {

        int minimum = INF;

        for (int j = 0; j < n; j++) {

            minimum = min(minimum, matrix[i][j]);

        }

        if (minimum == INF) continue;

        reduction += minimum;

        for (int j = 0; j < n; j++) {

            if (matrix[i][j] != INF) matrix[i][j] -= minimum;

        }

    }

    // Subtract the smallest finite value from every column.

    for (int j = 0; j < n; j++) {

        int minimum = INF;

        for (int i = 0; i < n; i++) {

            minimum = min(minimum, matrix[i][j]);

        }

        if (minimum == INF) continue;

        reduction += minimum;

        for (int i = 0; i < n; i++) {

            if (matrix[i][j] != INF) matrix[i][j] -= minimum;

        }

    }

    return reduction;

}

int main() {

    int n;

    cout << "Enter number of cities: ";

    cin >> n;

    if (n < 2) return 0;

    vector<vector<int>> cost(n, vector<int>(n));

    cout << "Enter cost matrix (0 for no road):\n";

    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            cin >> cost[i][j];

            if (i == j || cost[i][j] <= 0) cost[i][j] = INF;

        }

    }

    Node root;

    root.matrix = cost;

    root.path.push_back(0);

    root.bound = reduceMatrix(root.matrix);

    priority_queue<Node, vector<Node>, Compare> pq;

    pq.push(root);

    int best = INF;

    vector<int> answer;

    while (!pq.empty()) {

        Node current = pq.top();

        pq.pop();

        if (current.bound >= best) continue;

        int u = current.path.back();

        if ((int)current.path.size() == n) {

            if (cost[u][0] == INF) continue;

            int total = cost[u][0];

            for (int k = 1; k < n; k++) {

                total += cost[current.path[k - 1]][current.path[k]];

            }

            if (total < best) {

                best = total;

                answer = current.path;

            }

            continue;

        }

        for (int v = 1; v < n; v++) {

            bool visited = false;

            for (int city : current.path) {

                if (city == v) visited = true;

            }

            if (visited || current.matrix[u][v] == INF) continue;

            Node child = current;

            child.path.push_back(v);

            child.bound += current.matrix[u][v];

            for (int j = 0; j < n; j++) child.matrix[u][j] = INF;

            for (int i = 0; i < n; i++) child.matrix[i][v] = INF;

            if ((int)child.path.size() < n) child.matrix[v][0] = INF;

            child.bound += reduceMatrix(child.matrix);

            if (child.bound < best) pq.push(child);

        }

    }

    if (best == INF) {

        cout << "No possible tour\n";

    } else {

        cout << "\nMinimum cost: " << best << "\nRoute: ";

        for (int city : answer) cout << city << " -> ";

        cout << "0\n";

    }

    return 0;

}


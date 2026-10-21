
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>
using namespace std;

struct Item {
    int weight;
    int utility;
    int priority;
    bool used = false;
};

int main() {
    int n, trucks;

    cout << "\n========== DISASTER RELIEF RESOURCE ALLOCATION ==========\n";

    cout << "Enter number of relief items: ";
    cin >> n;

    if (!cin || n <= 0) {
        cout << "Invalid number of items.\n";
        return 1;
    }

    vector<Item> items(n);

    // Input relief items
    for (int i = 0; i < n; i++) {
        cout << "Item " << i + 1
             << " (weight utility priority [0/1]): ";

        cin >> items[i].weight
            >> items[i].utility
            >> items[i].priority;

        if (!cin || items[i].weight <= 0 ||
            items[i].utility < 0 ||
            (items[i].priority != 0 && items[i].priority != 1)) {
            cout << "Invalid item data.\n";
            return 1;
        }
    }

    // Display item inventory
    cout << "\n================ RELIEF ITEM INVENTORY ================\n";

    cout << left
         << setw(10) << "Item"
         << setw(15) << "Weight (kg)"
         << setw(12) << "Utility"
         << setw(12) << "Priority"
         << setw(15) << "Bonus"
         << setw(15) << "Final Utility"
         << "\n";

    cout << string(79, '-') << "\n";

    for (int i = 0; i < n; i++) {
        int bonus = items[i].priority == 1 ? 10 : 0;

        cout << left
             << setw(10) << i + 1
             << setw(15) << items[i].weight
             << setw(12) << items[i].utility
             << setw(12) << (items[i].priority == 1 ? "High" : "Normal")
             << setw(15) << bonus
             << setw(15) << items[i].utility + bonus
             << "\n";
    }

    cout << string(79, '-') << "\n";

    cout << "\nEnter number of trucks: ";
    cin >> trucks;

    if (!cin || trucks <= 0) {
        cout << "Invalid number of trucks.\n";
        return 1;
    }

    int grandTotalUtility = 0;
    int grandTotalWeight = 0;

    cout << "\nPriority bonus: +10 utility for high-priority items.\n";

    for (int t = 1; t <= trucks; t++) {
        int W;

        cout << "\nEnter capacity of truck " << t << " (kg): ";
        cin >> W;

        if (!cin || W < 0) {
            cout << "Invalid truck capacity.\n";
            return 1;
        }

        // Build bottom-up 0/1 Knapsack DP table
        vector<vector<int>> dp(n + 1, vector<int>(W + 1, 0));

        for (int i = 1; i <= n; i++) {
            for (int w = 0; w <= W; w++) {

                // Option 1: Skip item
                dp[i][w] = dp[i - 1][w];

                // Option 2: Include item if available and it fits
                if (!items[i - 1].used &&
                    items[i - 1].weight <= w) {

                    int value = items[i - 1].utility;

                    if (items[i - 1].priority == 1)
                        value += 10;

                    int include = value +
                        dp[i - 1][w - items[i - 1].weight];

                    dp[i][w] = max(dp[i][w], include);
                }
            }
        }

        // Backtrack and store selected items
        int w = W;
        vector<int> selected;

        for (int i = n; i >= 1; i--) {

            if (dp[i][w] != dp[i - 1][w]) {
                selected.push_back(i - 1);
                items[i - 1].used = true;
                w -= items[i - 1].weight;
            }
        }

        reverse(selected.begin(), selected.end());

        int totalWeight = 0;
        int totalUtility = 0;

        cout << "\n================ TRUCK " << t
             << " - SELECTED ITEMS ================\n";

        cout << left
             << setw(10) << "Item"
             << setw(15) << "Weight (kg)"
             << setw(12) << "Utility"
             << setw(12) << "Priority"
             << setw(12) << "Bonus"
             << setw(15) << "Final Utility"
             << "\n";

        cout << string(76, '-') << "\n";

        for (int index : selected) {
            int bonus = items[index].priority == 1 ? 10 : 0;
            int finalUtility = items[index].utility + bonus;

            totalWeight += items[index].weight;
            totalUtility += finalUtility;

            cout << left
                 << setw(10) << index + 1
                 << setw(15) << items[index].weight
                 << setw(12) << items[index].utility
                 << setw(12) << (items[index].priority == 1 ? "High" : "Normal")
                 << setw(12) << bonus
                 << setw(15) << finalUtility
                 << "\n";
        }

        if (selected.empty()) {
            cout << "No suitable items selected.\n";
        }

        cout << string(76, '-') << "\n";

        int remainingCapacity = W - totalWeight;

        cout << "\n---------------- TRUCK " << t << " SUMMARY ----------------\n";
        cout << "Truck Capacity       : " << W << " kg\n";
        cout << "Total Weight Loaded  : " << totalWeight << " kg\n";
        cout << "Remaining Capacity   : " << remainingCapacity << " kg\n";
        cout << "Items Selected       : " << selected.size() << "\n";
        cout << "Maximum Utility      : " << dp[n][W] << "\n";
        cout << "--------------------------------------------------\n";

        grandTotalWeight += totalWeight;
        grandTotalUtility += totalUtility;
    }

    // Final resource allocation summary
    cout << "\n=============== FINAL ALLOCATION SUMMARY ===============\n";

    cout << left
         << setw(25) << "Total Trucks"
         << trucks << "\n";

    cout << left
         << setw(25) << "Total Weight Loaded"
         << grandTotalWeight << " kg\n";

    cout << left
         << setw(25) << "Total Utility"
         << grandTotalUtility << "\n";

    cout << "\n================ REMAINING ITEMS ================\n";

    cout << left
         << setw(12) << "Item"
         << setw(15) << "Weight (kg)"
         << setw(15) << "Utility"
         << setw(15) << "Status"
         << "\n";

    cout << string(57, '-') << "\n";

    for (int i = 0; i < n; i++) {
        cout << left
             << setw(12) << i + 1
             << setw(15) << items[i].weight
             << setw(15) << items[i].utility
             << setw(15) << (items[i].used ? "Allocated" : "Available")
             << "\n";
    }

    cout << string(57, '-') << "\n";

    return 0;
}

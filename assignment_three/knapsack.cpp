
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Item {
    string name;
    double weight;
    double utility;
    double uw_ratio;
};

bool compare_ratio(Item a, Item b) {
    return a.uw_ratio > b.uw_ratio;
}

int main() {
    vector<Item> items;

    int choice;
    cout << "========== FRACTIONAL KNAPSACK ==========\n";
    cout << "1. Enter items manually\n";
    cout << "2. Read items from CSV\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1) {
        int n;
        cout << "Enter number of items: ";
        cin >> n;

        for (int i = 0; i < n; i++) {
            Item item;

            cout << "\nItem " << i + 1 << ":\n";
            cout << "Enter name: ";
            getline(cin >> ws, item.name);

            cout << "Enter weight (kg): ";
            cin >> item.weight;

            cout << "Enter utility: ";
            cin >> item.utility;

            if (item.weight <= 0 || item.utility < 0) {
                cout << "Invalid item weight or utility.\n";
                return 1;
            }

            items.push_back(item);
        }
    }
    else if (choice == 2) {
        string filename;
        cout << "Enter CSV filename: ";
        cin >> filename;

        ifstream file(filename);

        if (!file.is_open()) {
            cout << "Error opening CSV file.\n";
            return 1;
        }

        string line;
        getline(file, line); // Skip header

        while (getline(file, line)) {
            stringstream ss(line);
            string id, name, weight, utility;

            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, weight, ',');
            getline(ss, utility, ',');

            Item item;
            item.name = name;
            item.weight = stod(weight);
            item.utility = stod(utility);

            if (item.weight <= 0 || item.utility < 0) {
                cout << "Invalid item in CSV.\n";
                return 1;
            }

            items.push_back(item);
        }

        file.close();
    }
    else {
        cout << "Invalid choice.\n";
        return 1;
    }

    if (items.empty()) {
        cout << "No items available.\n";
        return 1;
    }

    double capacity;
    cout << "\nEnter boat capacity (kg): ";
    cin >> capacity;

    if (capacity <= 0) {
        cout << "Invalid capacity.\n";
        return 1;
    }

    // Calculate utility per kg
    for (auto &item : items) {
        item.uw_ratio = item.utility / item.weight;
    }

    // Sort by utility-to-weight ratio
    sort(items.begin(), items.end(), compare_ratio);

    double remaining_capacity = capacity;
    double total_utility = 0;
    double total_weight = 0;

    cout << fixed << setprecision(2);

    cout << "\n==================== RELIEF SUPPLY SELECTION ====================\n";

    cout << left
         << setw(20) << "Item"
         << right
         << setw(12) << "Weight"
         << setw(12) << "Utility"
         << setw(12) << "Ratio"
         << setw(16) << "Selected (kg)"
         << setw(14) << "Remaining"
         << setw(14) << "Utility +"
         << "\n";

    cout << string(100, '-') << "\n";

    for (auto &item : items) {
        double selected_weight = 0;
        double gained_utility = 0;

        if (remaining_capacity > 0) {

            if (item.weight <= remaining_capacity) {
                selected_weight = item.weight;
                gained_utility = item.utility;
            }
            else {
                double fraction = remaining_capacity / item.weight;

                selected_weight = remaining_capacity;
                gained_utility = item.utility * fraction;
            }

            remaining_capacity -= selected_weight;
            total_weight += selected_weight;
            total_utility += gained_utility;
        }

        cout << left << setw(20) << item.name
             << right
             << setw(12) << item.weight
             << setw(12) << item.utility
             << setw(12) << item.uw_ratio
             << setw(16) << selected_weight
             << setw(14) << remaining_capacity
             << setw(14) << gained_utility
             << "\n";
    }

    cout << string(100, '-') << "\n";

    cout << "\n==================== FINAL RESULT ====================\n";
    cout << "Boat Capacity       : " << capacity << " kg\n";
    cout << "Total Weight Loaded : " << total_weight << " kg\n";
    cout << "Remaining Capacity  : " << remaining_capacity << " kg\n";
    cout << "Maximum Utility     : " << total_utility << "\n";
    cout << "======================================================\n";

    return 0;
}

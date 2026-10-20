#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
struct Item {
   int weight;
   int utility;
   int priority;
   bool used = false;
};
int main() {
   int n, trucks;
   cout << "Enter number of items: ";
   cin >> n;
   vector<Item> items(n);
   for (int i = 0; i < n; i++) {
       cout << "Item " << i + 1
            << " (weight utility priority): ";
       cin >> items[i].weight
           >> items[i].utility
           >> items[i].priority;
   }
   cout << "Enter number of trucks: ";
   cin >> trucks;
   for (int t = 1; t <= trucks; t++) {
       int W;
       cout << "Enter capacity of truck " << t << ": ";
       cin >> W;
       vector<vector<int>> dp(n + 1,
                              vector<int>(W + 1, 0));
       // Build the bottom-up 0/1 knapsack DP table.
       for (int i = 1; i <= n; i++) {
           for (int w = 0; w <= W; w++) {
               dp[i][w] = dp[i - 1][w]; // Skip item.
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
       cout << "\nTruck " << t << "\n";
       cout << "Maximum utility: " << dp[n][W] << "\n";
       cout << "Selected items:\n";
       // Backtrack to identify the selected items.
       int w = W;
       for (int i = n; i >= 1; i--) {
           if (dp[i][w] != dp[i - 1][w]) {
               cout << "Item " << i << " ("
                    << items[i - 1].weight << " kg)\n";
               items[i - 1].used = true;
               w -= items[i - 1].weight;
           }
       }
       cout << "\n";
   }
   return 0;
}

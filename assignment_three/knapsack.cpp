#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
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

    ifstream file("data.csv");

    if(!file.is_open()){
        cout << "error : couldn't open the file"  << endl;
        return 1;
    }

    string line;
    getline(file, line);

    while(getline(file, line)) {
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

        items.push_back(item);
    }

    file.close();

    for(auto &item : items){
        item.uw_ratio = item.utility / item.weight;
    }

    sort(items.begin(), items.end(), compare_ratio);

    double capacity = 50;
    double remaining_capacity = capacity;
    double total_utility = 0;

    for(auto &item : items) {
        if(remaining_capacity <= 0) {
            break;
        }

        if(item.weight <= remaining_capacity) {
            total_utility += item.utility;
            remaining_capacity -= item.weight;

            cout << item.name << " : full item selected." << endl;
        }else {
            double fraction = remaining_capacity / item.weight;
            total_utility += item.utility * fraction;

            cout << item.name << " : fraction selected = " << fraction << endl;

            remaining_capacity = 0;
        }

    }
    cout << "maximum utility = " << total_utility << endl;

    return 0;
}
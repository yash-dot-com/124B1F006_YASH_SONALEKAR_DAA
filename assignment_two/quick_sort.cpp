#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector> // Required include for std::vector

using namespace std;

int partitionThis(vector<long long> &v, int start, int end);

// Recursive sorting function
void quickSort(vector<long long> &v, int start, int end){
    if(end <= start) return;
    
    int pivot = partitionThis(v, start, end);
    quickSort(v, start, pivot-1);
    quickSort(v, pivot+1, end);
}

int partitionThis(vector<long long> &v, int start, int end){
    int pivot = v[end];
    int i = start - 1;
    
    for(int j = start; j <= end - 1; j++){
        if(v[j] < pivot){
            i++;
            // Swap v[i] and v[j]
            int temp = v[i];
            v[i] = v[j];
            v[j] = temp;
        }
    }
    i++;
    // Place pivot in its correct sorted position
    int temp = v[i];
    v[i] = v[end];
    v[end] = temp;
    
    return i;
}

void printVector(const vector<long long> &v){
    for(auto i : v){
        cout << i << " ";
    }
    cout << endl;
}

int main(){
    vector<long long> timestamps;
    ifstream file("customer_orders.csv");

    if(!file.is_open()){
        cout << "error : couldn't open csv file" << endl;
        return 1;
    }

    string line;
    getline(file, line);

    while(getline(file, line)){
        stringstream ss(line);
        string order_id;
        string timestamp;

        getline(ss, order_id, ',');
        getline(ss, timestamp, ',');

        timestamps.push_back(stoll(timestamp));
    }
    file.close();

    cout << "records loaded." << endl;
    auto clock_start = chrono::high_resolution_clock::now();
    int start = 0;
    int end = timestamps.size() - 1; 
    quickSort(timestamps, start, end);
    auto clock_end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = clock_end - clock_start;

    cout << "sorting completed" << endl;
    cout << "execution time : " << elapsed.count() << " seconds." << endl;

    ofstream outputfile("sorted_customer_orders.csv");
    if(!outputfile.is_open()){
        cout << "error : couldn't create output file" << endl;
        return 1;
    }


    outputfile << "timestamp" << endl;

    for(auto timestamp : timestamps) {
        outputfile << timestamp << "\n";
    }

    outputfile.close();
    
    return 0;
}
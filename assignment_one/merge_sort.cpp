#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
using namespace std;

void merge(vector<long long> &leftV, vector<long long> &rightV, vector<long long> &v) {
    int leftSize = v.size() / 2;
    int rightSize = v.size() - leftSize;

    int i = 0;
    int l = 0;
    int r = 0;

    while(l < leftSize && r < rightSize){
        if(leftV[l] < rightV[r]){
            v[i] = leftV[l];
            i++;
            l++;
        }else{
            v[i] = rightV[r];
            i++;
            r++;
        }
    }

    while(l < leftSize) {
        v[i] = leftV[l];
        i++;
        l++;
    }

    while(r < rightSize) {
        v[i] = rightV[r];
        r++;
        i++;
    }
}

void mergeSort(vector<long long> &v) {
    int length = v.size();

    if(length <= 1){
        return;
    }

    int middle = length / 2;

    vector<long long> leftV(middle);
    vector<long long> rightV(length - middle);

    int i = 0;
    int j = 0;

    // insert items in left & right
    for(; i < length; i++){
        if(i < middle){
            leftV[i] = v[i];
        }else {
            rightV[j] = v[i];
            j++;
        }
    }

    mergeSort(leftV);
    mergeSort(rightV);

    merge(leftV, rightV, v);
}

void print_vector(vector<long long> &v){
    for(auto item : v){
        cout << item << " ";
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

    cout << "record loaded" << endl;
    auto start = chrono::high_resolution_clock::now();
    mergeSort(timestamps);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "sorting completed" << endl;
    cout << "execution time : " << elapsed.count() << " seconds" << endl;

    ofstream outputfile("sorted_customer_orders.csv");

    if(!outputfile.is_open()){
        cout << "error : couldn't not create output file";
        return 1;
    }

    outputfile << "timestamp" << endl;

    for(auto timestamp : timestamps){
        outputfile << timestamp << "\n";
    }

    outputfile.close();

    return 0;
}
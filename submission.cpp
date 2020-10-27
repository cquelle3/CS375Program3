#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <tuple>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>

using namespace std;

bool sorttuple(const tuple<int, int, float, int>& a, const tuple<int, int, float, int>& b){
    return (get<2>(a) > get<2>(b));
}

pair<int, vector<int>> greedy1(vector<tuple<int, int, float, int>> val, int capacity){
    
    int current_capacity = 0;
    int max_profit = 0;
    vector<int> ret;
    
    vector<tuple<int, int, float, int>> v = val;
    sort(v.begin(), v.end(), sorttuple);
    
    for(int i = 0; i < int(v.size()); i++){
        if(current_capacity + get<0>(v[i]) <= capacity){
            max_profit += get<1>(v[i]);
            current_capacity += get<0>(v[i]);
            ret.push_back(get<3>(v[i]) + 1);
        }
    }
    
    sort(ret.begin(), ret.end());
    
    pair<int, vector<int>> p = make_pair(max_profit, ret);
    
    return p;
}

pair<int, vector<int>> greedy2(vector<tuple<int, int, float, int>> val, int capacity){
    
    int max_profit = 0;
    int pmax = 0;
    
    pair<int, vector<int>> greedy1pair = greedy1(val, capacity);
    
    int greedy1_prof = get<0>(greedy1pair);
    
    vector<tuple<int, int, float, int>> v = val;
    sort(v.begin(), v.end(), sorttuple);
    
    int temp_index;
    
    for(int i = 0; i < int(v.size()); i++){
        if((get<1>(v[i]) > pmax) && (get<0>(v[i]) <= capacity)){
            pmax = get<1>(v[i]);
            temp_index = i;
        }
    }
    
    if(greedy1_prof > pmax){
        max_profit = greedy1_prof;
    }
    else{
        max_profit = pmax;
        get<1>(greedy1pair).clear();
        get<1>(greedy1pair).push_back(get<3>(v[temp_index]) + 1);
        sort(get<1>(greedy1pair).begin(), get<1>(greedy1pair).end());
    }
    
    pair<int, vector<int>> p = make_pair(max_profit, get<1>(greedy1pair));
    
    return p;
}

int KWF2(int i, int profit, int weight, int capacity, int * maxprofit, int * numbest, string bestset[], string include[], int p[], int w[], int n){
    
    int bound = profit;
    
    float x[n];
    for(int j = i; j < n; j++){
        x[j] = 0;
    }
    
    while(weight < capacity && i < n){
        if(weight + w[i] <= capacity){
            x[i] = 1;
            weight = weight + w[i];
            bound = bound + p[i];       
        }
        else{
            
            x[i] = (float(capacity) - float(weight)) / float(w[i]);
            
            
            weight = capacity;
            bound = bound + (p[i]*x[i]);
            
        }
        i = i + 1;
    }

    return bound;
}

bool promising(int i, int profit, int weight, int capacity, int * maxprofit, int * numbest, string bestset[], string include[], int p[], int w[], int n){
    if(weight >= capacity){
        return false;
    }
    
    int bound = KWF2(i, profit, weight, capacity, maxprofit, numbest, bestset, include, p, w, n);
    return (bound > *maxprofit);
}

void knapsack(int i, int profit, int weight, int capacity, int * maxprofit, int * numbest, string bestset[], string include[], int p[], int w[], int n){
    if(weight <= capacity && profit > *maxprofit){
        *maxprofit = profit;
        *numbest = i;
        for(int j = 1; j < n; j++){
            bestset[j] = include[j];
        }
    }
    if(promising(i+1, profit, weight, capacity, maxprofit, numbest, bestset, include, p, w, n)){
        include[i+1] = "yes";
        knapsack(i+1, profit + p[i+1], weight + w[i+1], capacity, maxprofit, numbest, bestset, include, p, w, n);
        
        include[i+1] = "no";
        knapsack(i+1, profit, weight, capacity, maxprofit, numbest, bestset, include, p, w, n);
    }
}

pair<int, vector<int>> bt_knapsack(vector<tuple<int, int, float, int>> val, int capacity){
    
    pair<int, vector<int>> greedy2pair = greedy2(val, capacity);
    
    int maxprofit = get<0>(greedy2pair);
    int og_maxprofit = maxprofit;
    vector<int> best_set = get<1>(greedy2pair);
    int numbest = 0;
    
    vector<tuple<int, int, float, int>> v = val;
    sort(v.begin(), v.end(), sorttuple);
    
    int arr_size = v.size() + 1;
    int w[arr_size];
    int p[arr_size];
    string * bestset = new string[arr_size];
    string * include = new string[arr_size];
    
    for(int i = 0; i < int(v.size()); i++){
        w[i+1] = get<0>(v[i]);
        p[i+1] = get<1>(v[i]);
        bestset[i+1] = ".";
        include[i+1] = ".";
    }
    
    knapsack(0, 0, 0, capacity, &maxprofit, &numbest, bestset, include, p, w, arr_size);

    if(maxprofit <= og_maxprofit){
        pair<int, vector<int>> ret = make_pair(maxprofit, best_set);
        return ret;
    }
    else{
        best_set.clear();
        for(int i = 1; i < arr_size; i++){
            if(bestset[i] == "yes"){
                best_set.push_back(get<3>(v[i-1]) + 1);
            }
        }
        sort(best_set.begin(), best_set.end());
        pair<int, vector<int>> ret = make_pair(maxprofit, best_set);
        return ret;
    } 
}

int main(int argc, char *argv[]) {
    
    ifstream infile(argv[1]);
    ofstream outfile(argv[2]);
    int mode = stoi(argv[3]);
    
    cout.rdbuf(outfile.rdbuf());

    string line;
    string s1;
    string s2;

    int count = 0;
    int capacity = 0;
    int input_size = 0;
    
    vector<tuple<int, int, float, int>> vals;
    
    while(getline(infile, line)){
        
        stringstream s(line);
        s >> s1 >> s2;
        
        if(count == 0){
            input_size = stoi(s1);
            count = stoi(s1);
            capacity = stoi(s2);
        }
        else{
            vals.push_back(make_tuple(stoi(s1), stoi(s2), stof(s2)/stof(s1), input_size - count));
            count--;
            
            if(count == 0){
                if(mode == 0){
                    auto start = chrono::high_resolution_clock::now();
                    pair<int, vector<int>> p = greedy1(vals, capacity);
                    auto end = chrono::high_resolution_clock::now();
                    auto time_needed = chrono::duration_cast<chrono::microseconds>(end - start).count();
                    
                    cout << input_size << " " << get<0>(p) << " " << time_needed << " ";
                    vector<int> v = get<1>(p);
                    for(int i = 0; i < int(v.size()); i++){
                        cout << v[i] << " ";
                    }
                    cout << endl;
                }
                else if(mode == 1){
                    auto start = chrono::high_resolution_clock::now();
                    pair<int, vector<int>> p = greedy2(vals, capacity);
                    auto end = chrono::high_resolution_clock::now();
                    auto time_needed = chrono::duration_cast<chrono::microseconds>(end - start).count();

                    cout << input_size << " " << get<0>(p) << " " << time_needed << " ";
                    vector<int> v = get<1>(p);
                    for(int i = 0; i < int(v.size()); i++){
                        cout << v[i] << " ";
                    }
                    cout << endl;
                }
                else{
                    auto start = chrono::high_resolution_clock::now();
                    pair<int, vector<int>> p = bt_knapsack(vals, capacity);
                    auto end = chrono::high_resolution_clock::now();
                    auto time_needed = chrono::duration_cast<chrono::microseconds>(end - start).count();

                    cout << input_size << " " << get<0>(p) << " " << time_needed << " ";
                    vector<int> v = get<1>(p);
                    for(int i = 0; i < int(v.size()); i++){
                        cout << v[i] << " ";
                    }
                    cout << endl;
                }
                vals.clear();
            }
        }
    }
    
    return 0;
}

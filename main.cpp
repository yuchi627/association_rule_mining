#include<iostream>
#include<map>
#include<set>
#include<sstream> 
#include<fstream>
#include <vector>
using namespace std;

float preprocess(map<set<int>, set<int>> &feq_item, map<int, int> &item_count, float min_supp){
    string filename = "input.txt";
    // string filename = "input_test.txt";
    ifstream file(filename);
    string str;
    int transaction_index = 0;
    const char delim = ' ';
    map<set<int>, set<int>> feq_item_ori;
    // ---------------- split data -----------------
    while (getline(file, str)) {
        set<int> out;
        // construct a stream from the string 
        stringstream ss(str); 
        string s; 
        while (std::getline(ss, s, delim)) { 
            set<int> item_set{stoi(s)};
            if (feq_item_ori.find(item_set) != feq_item_ori.end()){ 
                feq_item_ori[item_set].insert(transaction_index);
                item_count[stoi(s)] = 1;
            }
            else{
                set<int> trans_set{transaction_index};
                feq_item_ori[item_set] = trans_set;
                item_count[stoi(s)] ++;
            }
        } 
        transaction_index ++;
    }
    
    float trans_amount = float(transaction_index);
    for (auto it = feq_item_ori.begin(); it != feq_item_ori.end(); ++it){
        if(it->second.size()/trans_amount < min_supp){
            for (const auto &s : it->first) {
                item_count.erase(s);
            }   
        }
        else
            feq_item[it->first] = it->second;
    }
    return trans_amount;
}

template<typename T>
void pretty_print(const T& input_seq, int combo, map<set<int>, set<set<int>>> &permutation_set)
{
    set<int> tmp1;
    set<int> tmp2;
    int n = input_seq.size();
    bool flag = true;
    for (int i = 0; i < n; ++i) {
        if ((combo >> i) & 1){
            if(flag){
                tmp1.insert(input_seq[i]);
                flag = false;
            }
            else
                tmp2.insert(input_seq[i]);
        }
    }
    if (permutation_set.find(tmp1) != permutation_set.end()){ 
        permutation_set[tmp1].insert(tmp2);
    }
    else{
        set<set<int>> tmp2_tmp{tmp2};
        permutation_set[tmp1] = tmp2_tmp;
    }
}

template<typename T>
void combo(const T& input_seq, int k, map<set<int>, set<set<int>>> &permutation_set)
{
    // ------- combination and permutation ---------
    permutation_set.clear();

    int n = input_seq.size();
    int combo = (1 << k) - 1;       // k bit sets
    while (combo < 1<<n) {
        pretty_print(input_seq, combo, permutation_set);
        int x = combo & -combo;
        int y = combo + x;
        int z = (combo & ~y);
        combo = z / x;
        combo >>= 1;
        combo |= y;
    }
}
 
bool permutation_preprocess(map<int, int> &item_count, vector<int> &per_item, int per_thres){
    // ------ remove the item less than permutation threshold ------
    per_item.clear();
    for (auto it = item_count.begin(); it != item_count.end(); ++it){
        if(it->second >= per_thres){
            per_item.push_back(it->first);
        }
    }
    bool flag = false;
    if(per_item.size() >= per_thres)
        flag = true;
    return flag;
}

void find_freq(map<set<int>, set<int>> &feq_item){

}

int main(){
    map<set<int>, set<int>> feq_item;
    vector<int> per_item;
    map<set<int>, set<set<int>>> permutation_set;
    map<int, int> item_count;
    bool cont_flag = false;
    float min_supp;
    float min_conf;
    cout << "Enter min_supp(%): ";
    cin >> min_supp;
    min_supp /= 100;
    cout << "Enter min_conf(%): ";
    cin >> min_conf;
    min_conf /= 100;
    float trans_amount = preprocess(feq_item, item_count, min_supp);
    int choice_amount = 1;
    cont_flag = permutation_preprocess(item_count, per_item, choice_amount);
    while(cont_flag){
        item_count.clear();
        combo(per_item, choice_amount+1, permutation_set);
        // ------ find all Frequent itemsets ------
        for (auto it = permutation_set.begin(); it != permutation_set.end(); ++it){
            for (const auto &s : it->second) {
                set<int> inter_set;
                set_intersection(feq_item[it->first].begin(), feq_item[it->first].end(), feq_item[s].begin(), feq_item[s].end(), insert_iterator<set<int>>(inter_set,inter_set.begin()));
                if(inter_set.size()/trans_amount >= min_supp){
                    set<int> union_set;
                    set_union(it->first.begin(), it->first.end(), s.begin(), s.end(), insert_iterator<set<int>>(union_set,union_set.begin()));
                    feq_item[union_set] = inter_set;
                    for (const auto &s : union_set)
                        item_count[s] += 1;
                }
            }
        }
        choice_amount ++;  
        cont_flag = permutation_preprocess(item_count, per_item, choice_amount);
    }
    ofstream output_file;
    output_file.open("out.txt");
    // ------ use Frequent itemsets to find Strong Rules ------
    for (auto it1 = feq_item.begin(); it1 != feq_item.end(); ++it1){
        for (auto it2 = feq_item.begin(); it2 != feq_item.end(); ++it2){
            set<int> item_inter_set;
            set_intersection(it1->first.begin(), it1->first.end(), it2->first.begin(), it2->first.end(), insert_iterator<set<int>>(item_inter_set,item_inter_set.begin()));
            if(item_inter_set.size() == 0){
                set<int> inter_set;
                set_intersection(it1->second.begin(), it1->second.end(), it2->second.begin(), it2->second.end(), insert_iterator<set<int>>(inter_set,inter_set.begin()));
                if (float(inter_set.size())/it1->second.size() > min_conf){
                    output_file << "{";
                    cout << "{";
                    bool print_flag = true;
                    for (auto it = it1->first.begin(); it != it1->first.end(); ++it) {
                        if(print_flag){
                            output_file << *it;
                            cout << *it;
                            print_flag = false;
                        }
                        else{
                            output_file << ", "<< *it;
                            cout << ", "<< *it;
                        }
                    }
                    output_file << "} -> {";
                    cout << "} -> {";
                    print_flag = true;
                    for (auto it = it2->first.begin(); it != it2->first.end(); ++it) {
                        if(print_flag){
                            output_file << *it;
                            cout << *it;
                            print_flag = false;
                        }
                        else{
                            output_file << ", "<< *it;
                            cout << ", "<< *it;
                        }
                    }
                    output_file << "}  (" << inter_set.size() << " / " << it1->second.size() << ")" << endl;
                    cout << "}  (" << inter_set.size() << " / " << it1->second.size() << ")" << endl;
                }
            }
        }    
    }

    system("pause");
    return 0;
}

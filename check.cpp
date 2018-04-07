#include <iostream>
#include <cstring>
#include <random>
#include <vector>
#include <fstream>
#include <string>

using namespace std;
using ll = long long;

const size_t N = 1005, K_origin = 150;

uint32_t index_num, edge_num;
vector<size_t > graph[N];
vector<size_t > Sol;
size_t Adjacent_Color_Table[N][K_origin];


inline ll cal_f() {
    ll rec = 0;
    for (size_t i = 1; i < index_num + 1 ; ++i) rec += Adjacent_Color_Table[i][Sol[i]];
    return rec / 2;
}

inline int helper() {
    memset(Adjacent_Color_Table, 0 , sizeof(Adjacent_Color_Table));
    for (size_t i = 1 ; i < index_num + 1 ; ++i)
        for (auto &r : graph[i])
            Adjacent_Color_Table[i][Sol[r]]++;
    return cal_f() ;
}

int main(int argc, char ** argv) {
    if (argc == 1) return -1;
    size_t u, v, sol;
    string s;
    bool needHea;
    clock_t startTime, endTime;

    streambuf *backup;
    ifstream fin;
    ofstream fout;
    fin.open(argv[1]);
    cout << argv[1] <<endl;
    backup = cin.rdbuf();
    cin.rdbuf(fin.rdbuf());

    while (cin >> s)
        if (s == "p") break;
    cin >> s >> index_num >> edge_num;
    while (true) {
        if (cin.eof()) break;
        cin >> s >> u >> v;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }
    // cout << "  dd" << endl;
    fin.close();
    fin.open(argv[2]);
    
    while (!fin.eof()) {
        cout << argv[2] << endl;
        for (size_t i = 0 ; i < index_num + 1; ++i) {
            fin >> sol;
            Sol.push_back(sol);
            cout << sol << " ";
        }    
        cout << endl;

        ll res = helper();

        if (res == 0) {
            cout << "no conflicts!" << endl;
        } else cout << "failed" << endl;
    }

    fin.close();
    cin.rdbuf(backup);
}
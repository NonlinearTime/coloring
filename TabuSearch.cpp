#include <iostream>
#include <cstring>
#include <random>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

#define file "/home/haimin/CLionProjects/yunchouxiu/instances/DSJC500.1.col"
#define log "/home/haimin/CLionProjects/yunchouxiu/log.txt"
#define Seed 2018
#define MaxIter 300000000
#define inf 0x7fffffff

typedef vector<size_t > Sol_t;
typedef long long ll;

using Engine = default_random_engine;

const size_t N = 505, K_origin = 130;

Sol_t Sol, BestSol;
ll f, Best_f;
size_t K;
size_t TabuTenure[N][K_origin];
size_t Adjacent_Color_Table[N][K_origin];


uint32_t index_num, edge_num;

vector<size_t > graph[N];
vector<size_t > equ_point, equ_color;
vector<size_t > equ_tabu_point, equ_tabu_color;

size_t point_equ[N * K_origin][2], tabu_point[N * K_origin][2];

ofstream fout;
unsigned long _seed;

Engine e;


struct Move {
    size_t u, vi, vj;
    int delt;
};

inline ll cal_f() {
    ll rec = 0;
    for (size_t i = 1; i < index_num + 1 ; ++i) rec += Adjacent_Color_Table[i][Sol[i]];
    return rec / 2;
}

inline int cal_delta(size_t u, size_t vi, size_t vj) {
    return static_cast<int>(Adjacent_Color_Table[u][vj]) - static_cast<int>(Adjacent_Color_Table[u][vi]);
}

void init() {
    _seed = clock();
    e.seed(_seed);
    memset(TabuTenure, 0, sizeof(TabuTenure));
    memset(Adjacent_Color_Table, 0 , sizeof(Adjacent_Color_Table));
    Sol.clear();
    for (size_t i = 0 ; i <= index_num ; ++i) {
        Sol.push_back(e() % K);
    }
    for (size_t i = 1 ; i < index_num + 1 ; ++i)
        for (auto &r : graph[i])
            Adjacent_Color_Table[i][Sol[r]]++;
    f = cal_f() ;
    Best_f = f;
}

int OneMove(size_t iter) {
    size_t u, vi, vj;
    int delt;
    size_t equ_count = 0, equ_tabu_count = 0;
    // equ_point.clear(), equ_color.clear();
    // equ_tabu_color.clear(), equ_tabu_point.clear();
    int tabu_delta = inf, delta = inf, tmp;
    // cout << "6" << endl;
    for (size_t i = 1 ; i < index_num + 1 ; ++i ) {
//        cout << "1" << " i:" << i << " SolSize:"<< Sol.size() << endl;
        if (Adjacent_Color_Table[i][Sol[i]] > 0) {
            for (size_t k = 0 ; k < K ; ++k) {
                if (k != Sol[i]) {
                    tmp = cal_delta(i, Sol[i], k);
                    if (iter < TabuTenure[i][k]) {
                        if (tmp <= tabu_delta) {
                            if (tmp < tabu_delta) {
                                tabu_delta = tmp;
                                equ_tabu_count = 0;
                                // equ_tabu_color.clear();
                                // equ_tabu_point.clear();
                            }
                            tabu_point[equ_tabu_count][0] = i;
                            tabu_point[equ_tabu_count++][1] = k;
                            // equ_tabu_point.push_back(i);
                            // equ_tabu_color.push_back(k);
                        }
                    } else {
                        if (tmp <= delta) {
                            if (tmp < delta) {
                                delta = tmp;
                                equ_count = 0;
                                // equ_point.clear();
                                // equ_color.clear();
                            }
                            // cout << "equ_cout: " << equ_count << endl;
                            point_equ[equ_count][0] = i;
                            point_equ[equ_count++][1] = k;
                            // equ_point.push_back(i);
                            // equ_color.push_back(k);
                        }
                    }
                }
            }
        }
    }
    // cout << "7" << endl;
    if (tabu_delta < Best_f - f && tabu_delta < delta) {
        // unsigned long index = e() % equ_tabu_point.size();
        // u = equ_tabu_point[index];
        // vi = Sol[equ_tabu_point[index]];
        // vj = equ_tabu_color[index];
        unsigned long id = e() % equ_tabu_count;
        // cout << " 2" << endl;
        u = tabu_point[id][0];
        vi = Sol[tabu_point[id][0]];
        vj = tabu_point[id][1];
        delt = tabu_delta;
    }
    else {
    //    cout << "size: " << equ_point.size() << endl;
        // cout << " 3 " << equ_count << " " << tabu_delta << " " << delta << endl;
        // cout << equ_point.size() << endl;
        // unsigned long index = e() % equ_point.size();
        // cout << index << endl;
        // u = equ_point[index];
        
        // vi = Sol[equ_point[index]];
        // vj = equ_color[index];
        // cout << "u: " << u << " vi: " << vi << " vj: " << vj << endl;
        
        unsigned long id = e() % equ_count;
        u = point_equ[id][0];
        vi = Sol[point_equ[id][0]];
        vj = point_equ[id][1];
        delt = delta;
    }
    // cout << "4" << endl;
    Sol[u] = vj;
//    cout << m.u << " " << m.vi << " " << m.vj << " " << m.delt << endl;
    f += delt;
    Best_f = f < Best_f ? f : Best_f;
//    cout << "f: " << f << endl;
    TabuTenure[u][vi] = iter + f + e() % 10 + 1;
    for (auto &r : graph[u]) {
        Adjacent_Color_Table[r][vi]--;
        Adjacent_Color_Table[r][vj]++;
    }
    // cout << "5" << endl;

    return delt;
}

void makeMove(const Move & m, size_t iter) {
    Sol[m.u] = m.vj;
//    cout << m.u << " " << m.vi << " " << m.vj << " " << m.delt << endl;
    f += m.delt;
    Best_f = f < Best_f ? f : Best_f;
//    cout << "f: " << f << endl;
    TabuTenure[m.u][m.vi] = iter + f + e() % 12 + 1;
    for (auto &r : graph[m.u]) {
        Adjacent_Color_Table[r][m.vi]--;
        Adjacent_Color_Table[r][m.vj]++;
    }
}

void TabuSearch() {
    size_t iter = 0;
    size_t cnt = 0;
    int delta;
    ll f_rec = Best_f;
    init();
    while (true) {
        delta = OneMove(iter);
        // makeMove(m,iter);
        iter++;

        if (Best_f == f_rec) cnt++;
        else {
            cnt = 0;
            f_rec = Best_f;
        }
        if (cnt == MaxIter || Best_f == 0) {
            fout << "Seed: " << _seed << endl;
            cout << "K: " << K
                 << " Seed: " << _seed << endl
                 << "Iter: " << iter
                 << " best_f: " << Best_f
                 << " delta: " << delta
                 << endl;
            break;
        }
    }
}

int main() {
    size_t u, v;
    string s;
    clock_t startTime, endTime;

    streambuf *backup;
    ifstream fin;
    fin.open(file);
    backup = cin.rdbuf();
    cin.rdbuf(fin.rdbuf());

    fout.open(log);

    while (cin >> s)
        if (s == "p") break;
    cin >> s >> index_num >> edge_num;
    while (true) {
        if (cin.eof()) break;
        cin >> s >> u >> v;
        // cout << s << " " << u << " " << v << endl;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    K = K_origin;
    // e.seed(clock());
    while (K--) {
        startTime = clock();
        TabuSearch();
        endTime = clock();
        if (!cal_f()) BestSol = Sol;
        else break;
        cout << "Total Time : " << (double)(endTime - startTime) / 1000 << "ms" << endl;
        fout << "K: " << K << " Best_f: " << Best_f << endl;
    }

    cout << "finished with best k : " << K + 1<< endl;
    fout << "finished with best k : " << K + 1<< endl;
    for (auto &r : BestSol) fout << " " << r;
    fout << endl;

    cin.rdbuf(backup);
    return 0;

}

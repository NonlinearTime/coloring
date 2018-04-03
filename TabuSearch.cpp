#include <iostream>
#include <cstring>
#include <random>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

#define file "/home/haimin/CLionProjects/yunchouxiu/instances/DSJC500.9.col"
#define log "/home/haimin/CLionProjects/yunchouxiu/log.txt"
#define Seed 2018
#define MaxIter 40000000
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

ofstream fout;
unsigned long _seed;

Engine e;

struct Move {
    size_t u, vi, vj;
    int delt;
};

ll cal_f() {
    ll rec = 0;
    for (size_t i = 1; i < index_num + 1 ; ++i) rec += Adjacent_Color_Table[i][Sol[i]];
    return rec / 2;
}

int cal_delta(const Move & m) {
    return static_cast<int>(Adjacent_Color_Table[m.u][m.vj]) - static_cast<int>(Adjacent_Color_Table[m.u][m.vi]);
}

void init() {
    _seed = e() % 1000;
    e.seed(_seed);
    memset(TabuTenure, 0, sizeof(TabuTenure));
    memset(Adjacent_Color_Table, 0 , sizeof(Adjacent_Color_Table));
    Sol.clear();
    for (size_t i = 0 ; i <= index_num ; ++i) {
        Sol.push_back(e() % K);
        BestSol.push_back(0);
    }
    for (size_t i = 1 ; i < index_num + 1 ; ++i)
        for (auto &r : graph[i])
            Adjacent_Color_Table[i][Sol[r]]++;
    f = cal_f() ;
    Best_f = f;
}

Move findMove(size_t iter) {
    Move m;
    Move best_move;
    equ_point.clear(), equ_color.clear();
    equ_tabu_color.clear(), equ_tabu_point.clear();
    memset(&best_move,0, sizeof(best_move));
    int tabu_delta = inf, delta = inf, tmp;
    for (size_t i = 1 ; i < index_num + 1 ; ++i ) {
//        cout << "1" << " i:" << i << " SolSize:"<< Sol.size() << endl;
        if (Adjacent_Color_Table[i][Sol[i]] > 0) {
            for (size_t k = 0 ; k < K ; ++k) {
                if (k != Sol[i]) {
                    m.u = i, m.vi = Sol[i], m.vj = k;
                    tmp = cal_delta(m);
                    if (iter < TabuTenure[i][k]) {
                        if (tmp <= tabu_delta) {
                            if (tmp < tabu_delta) {
                                tabu_delta = tmp;
                                equ_tabu_color.clear();
                                equ_tabu_point.clear();
                            }
                            equ_tabu_point.push_back(i);
                            equ_tabu_color.push_back(k);
                        }
                    } else {
                        if (tmp <= delta) {
                            if (tmp < delta) {
                                delta = tmp;
                                equ_point.clear();
                                equ_color.clear();
                            }
                            equ_point.push_back(i);
                            equ_color.push_back(k);
                        }
                    }
                }
            }
        }
    }
    if (tabu_delta < Best_f - f && tabu_delta < delta) {
        unsigned long index = e() % equ_tabu_point.size();
        best_move.u = equ_tabu_point[index];
        best_move.vi = Sol[equ_tabu_point[index]];
        best_move.vj = equ_tabu_color[index];
        best_move.delt = tabu_delta;
    }
    else {
//        cout << "size: " << equ_point.size() << endl;
        unsigned long index = e() % equ_point.size();
        best_move.u = equ_point[index];
        best_move.vi = Sol[equ_point[index]];
        best_move.vj = equ_color[index];
        best_move.delt = delta;
    }
    return best_move;
}

void makeMove(const Move & m, size_t iter) {
    Sol[m.u] = m.vj;
//    cout << m.u << " " << m.vi << " " << m.vj << " " << m.delt << endl;
    f += m.delt;
    Best_f = f < Best_f ? f : Best_f;
//    cout << "f: " << f << endl;
    TabuTenure[m.u][m.vi] = iter + f + e() % 10 + 1;
    for (auto &r : graph[m.u]) {
        Adjacent_Color_Table[r][m.vi]--;
        Adjacent_Color_Table[r][m.vj]++;
    }
}

void TabuSearch() {
    Move m;
    size_t iter = 0;
    size_t cnt = 0;
    ll f_rec = Best_f;
    init();
    while (true) {
        m = findMove(iter);
        makeMove(m,iter);
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
                 << " delta: " << m.delt
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
        cout << s << " " << u << " " << v << endl;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    K = K_origin;
    e.seed(Seed);
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

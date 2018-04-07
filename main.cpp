#include <iostream>
#include <cstring>
#include <random>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

#define file "/home/haimin/CLionProjects/yunchouxiu/instances/DSJC500.5.col"
#define log "/home/haimin/CLionProjects/yunchouxiu/log.txt"
#define Seed 2018
#define MaxIterInit 1000000
#define MaxIterCross 80000
#define population 10
#define MaxSameIter 500000
#define inf 0x7fffffff

typedef vector<size_t > Sol_t;
typedef long long ll;

using Engine = default_random_engine;

const size_t N = 505, K_origin = 55;

Sol_t Sol, BestSol;
Sol_t Sol_Rec[population + 1];
ll f, Best_f, rec_f, worst_f, f_sol_best;
ll f_sol[N];
size_t K, min_index;
size_t TabuTenure[N][K_origin];
size_t Adjacent_Color_Table[N][K_origin];
size_t isCollected[N];
size_t point_equ[N * K_origin][2], tabu_point[N * K_origin][2];

uint32_t index_num, edge_num;

vector<size_t > graph[N];
vector<size_t > equ_point, equ_color;
vector<size_t > equ_tabu_point, equ_tabu_color;
vector<size_t > V_1[K_origin];
vector<size_t > V_2[K_origin];


ofstream fout;
unsigned long _seed;

Engine e;

struct Move {
    size_t u, vi, vj;
    int delt;
};

inline ll cal_f(size_t index) {
    ll rec = 0;
    for (size_t i = 1; i < index_num + 1 ; ++i) rec += Adjacent_Color_Table[i][Sol_Rec[index][i]];
    return rec / 2;
}

inline int cal_delta(size_t u, size_t vi, size_t vj) {
    return static_cast<int>(Adjacent_Color_Table[u][vj]) - static_cast<int>(Adjacent_Color_Table[u][vi]);
}


inline void solCopy(Sol_t src, Sol_t dst) {
    for (size_t i = 0; i != src.size() ; ++i) {
        src[i] = dst[i];
    }
}

void initialPopulation() {
    for (size_t k = 0 ; k < population + 1 ; ++k) Sol_Rec[k].clear();
    for (size_t i = 0 ; i <= index_num ; ++i) BestSol.push_back(e() % K);
    for (size_t k = 0 ; k < population + 1 ; ++k) {
        for (size_t i = 0 ; i <= index_num ; ++i) {
            // printf("i: %d k: %d\n", i, k);
            Sol_Rec[k].push_back(e() % K);
        }
    }
}

inline void init(size_t index) {
    _seed = clock();
    e.seed(_seed);
    memset(TabuTenure, 0, sizeof(TabuTenure));
    memset(Adjacent_Color_Table, 0 , sizeof(Adjacent_Color_Table));
//    Sol_Rec[index].clear();
//    for (size_t i = 0 ; i <= index_num ; ++i) {
//        Sol_Rec[index].push_back(e() % K);
//    }
    for (size_t i = 1 ; i < index_num + 1 ; ++i)
        for (auto &r : graph[i])
            Adjacent_Color_Table[i][Sol_Rec[index][r]]++;
    f = cal_f(index) ;
    Best_f = f;
}

int findMove(size_t iter, size_t index) {
    size_t u, vi, vj;
    int delt;
    size_t equ_count = 0, equ_tabu_count = 0;
    // equ_point.clear(), equ_color.clear();
    // equ_tabu_color.clear(), equ_tabu_point.clear();
    int tabu_delta = inf, delta = inf, tmp;
    for (size_t i = 1 ; i < index_num + 1 ; ++i ) {
//        cout << "1" << " i:" << i << " SolSize:"<< Sol.size() << endl;
        if (Adjacent_Color_Table[i][Sol_Rec[index][i]] > 0) {
            for (size_t k = 0 ; k < K ; ++k) {
                if (k != Sol_Rec[index][i]) {
                    u = i, vi = Sol_Rec[index][i], vj = k;
                    tmp = cal_delta(u, vi, vj);
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
    if (tabu_delta < Best_f - f && tabu_delta < delta) {
        // unsigned long id = e() % equ_tabu_point.size();
        unsigned long id = e() % equ_tabu_count;
        u = tabu_point[id][0];
        vi = Sol_Rec[index][tabu_point[id][0]];
        vj = tabu_point[id][1];
        delt = tabu_delta;
    }
    else {
//        cout << "size: " << equ_point.size() << endl;
        unsigned long id = e() % equ_count;
        u = point_equ[id][0];
        vi = Sol_Rec[index][point_equ[id][0]];
        vj = point_equ[id][1];
        delt = delta;
    }
    Sol_Rec[index][u] = vj;
//    cout << m.u << " " << m.vi << " " << m.vj << " " << m.delt << endl;
    f += delt;
    if (f < Best_f) {
        Best_f = f;
        solCopy(BestSol, Sol_Rec[index]);
    }
//    cout << "f: " << f << endl;
    TabuTenure[u][vi] = iter + f + e() % 10 + 1;
    for (auto &r : graph[u]) {
        Adjacent_Color_Table[r][vi]--;
        Adjacent_Color_Table[r][vj]++;
    }
    return delt;
}

void makeMove(const Move & m, size_t iter, size_t index) {
    Sol_Rec[index][m.u] = m.vj;
//    cout << m.u << " " << m.vi << " " << m.vj << " " << m.delt << endl;
    f += m.delt;
    if (f < Best_f) {
        Best_f = f;
        solCopy(BestSol, Sol_Rec[index]);
    }
//    cout << "f: " << f << endl;
    TabuTenure[m.u][m.vi] = iter + f + e() % 10 + 1;
    for (auto &r : graph[m.u]) {
        Adjacent_Color_Table[r][m.vi]--;
        Adjacent_Color_Table[r][m.vj]++;
    }
}

void TabuSearch(size_t index, size_t iterNum) {
    int m;
    size_t iter = 0;
    size_t cnt = 0;
    ll f_rec = Best_f;
    init(index);
    while (iter < iterNum) {
        m = findMove(iter, index);
        // makeMove(m, iter, index);
        iter++;
        if (Best_f == f_rec) cnt++;
        else {
            cnt = 0;
            f_rec = Best_f;
        }
        if (Best_f == 0) {
            fout << "Seed: " << _seed << endl;
            cout << "K: " << K
                 << " Seed: " << _seed << endl
                 << "Iter: " << iter
                 << " best_f: " << Best_f
                 << " delta: " << m
                 << endl;
            break;
        }
    }
    solCopy(Sol_Rec[index], BestSol);
    f_sol[index] = Best_f;
}

void crossoverOperator(size_t fst, size_t snd) {
    uint64_t fst_max = 0, snd_max = 0;
    size_t fst_rec = 0, snd_rec = 0;
    for (size_t k = 0 ; k < K ; ++k) {
        for (size_t i = 0; i < K + 1; ++i) {
            V_1[i].clear();
            V_2[i].clear();
        }
        memset(isCollected, 0, N);
        for (size_t i = 1; i < index_num + 1; ++i) {
            if (!isCollected[i]) {
                V_1[Sol_Rec[fst][i]].push_back(i);
                V_2[Sol_Rec[snd][i]].push_back(i);
            }
        }
        for (size_t i = 1; i < K + 1; ++i) {
            if (V_1[i].size() > fst_max) {
                fst_max = V_1[i].size();
                fst_rec = i;
            }
            if (V_2[i].size() > snd_max) {
                snd_max = V_2[i].size();
                snd_rec = i;
            }
        }
        if (k % 2) {
            for (auto &r: V_1[fst_rec]) {
                Sol_Rec[0][r] = k; //fst_rec
                isCollected[r] = 1;
            }
        } else {
            for (auto &r: V_2[snd_rec]) {
                Sol_Rec[0][r] = k; //snd_rec
                isCollected[r] = 1;
            }
        }
    }
    // for (size_t i = 1 ; i < index_num + 1 ; ++i) {
    //     if (!isCollected[i]) Sol_Rec[0][i] = BestSol[i];
    // }
    for (size_t i = 1 ; i < index_num + 1 ; ++i) {
        if (!isCollected[i]) Sol_Rec[0][i] = e() % K;
    }
    // uint64_t tmp = cal_f(0);
    // for (size_t i = 1 ; i < index_num + 1 ; ++i) {
    //     if (!isCollected[i]) Sol_Rec[0][i] = e() % K;
    // }
    // if (tmp < cal_f(0)) {
    //     for (size_t i = 1 ; i < index_num + 1 ; ++i) {
    //         if (!isCollected[i]) Sol_Rec[0][i] = BestSol[i];
    //     }
    // }

}

void poolUpdate() {
    Sol_t index;
    worst_f = 0;
    for (size_t i = 0 ; i < population + 1; ++i) {
        if (f_sol[i] >= worst_f) {
            if (f_sol[i] > worst_f) {
                worst_f = f_sol[i];
                index.clear();
            }
            index.push_back(i);
        }
    }

    size_t id = e() % index.size();
    if (worst_f >= f_sol[0]) {
        f_sol[index[id]] = f_sol[0];
        solCopy(Sol_Rec[index[id]], Sol_Rec[0]);
    }
}

int main() {
    size_t u, v;
    string s;
    bool needHea;
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
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    K = K_origin;
    e.seed(clock());
    while (K--) {
        startTime = clock();
        rec_f = inf;
        needHea = false;
        initialPopulation();
        for (size_t i = 1; i < population + 1; ++i) {
            TabuSearch(i, MaxIterInit);
            if (f_sol[i] < rec_f) {
                rec_f = f_sol[i];
                solCopy(BestSol,Sol_Rec[i]);
                min_index = i;
                cout << "min_index : " << min_index << " rec_f: " << f_sol[min_index] << endl;
            }
            cout << "Sol_Rec[" << i << "]: " << " f: " << f_sol[min_index] << endl;
            if (!f_sol[i]) {
                needHea = true;
                endTime = clock();
                cout << "Total Time : " << (double) (endTime - startTime) / 1000 << "ms" << endl;
                fout << "K: " << K << " Best_f: " << Best_f << endl;
                break;
            }
        }
        cout << "Min index : " << min_index << " f: " << f_sol[min_index] << " Sol_size: " << Sol_Rec[min_index].size() << endl;
        f_sol_best = f_sol[min_index];
        if (!needHea) {
            while (true) {
                _seed = e();
                e.seed(_seed);
                size_t fst, snd;
                fst = e() % population + 1;
                while (true) {
                    snd = e() % population + 1;
                    if (snd != fst) break;
                }
                cout << "fa: " << f_sol[fst] << " mo: " << f_sol[snd] << endl;
                crossoverOperator(fst, snd);
                TabuSearch(0, MaxIterCross);
                cout << "cal_f: " << f_sol[0] << endl;
                if (f_sol[0] <= f_sol_best) {
                    f_sol_best = f_sol[0];
                    solCopy(BestSol, Sol_Rec[0]);
                }

                poolUpdate();

                cout << "K: " << K << " Best_f: " << f_sol_best << endl;
                for (size_t i = 0 ; i < population ; ++i) {
                    cout << f_sol[i] << " " ;
                }
                cout << f_sol[population] << endl;
                if (f_sol_best == 0) {
                    endTime = clock();
                    cout << "Total Time : " << (double) (endTime - startTime) / (1000*1000) << "s" << endl;
                    fout << "Total Time : " << (double) (endTime - startTime) / (1000*1000) << "s" << endl;
                    fout << "K: " << K << " Best_f: " << Best_f << endl;
                    break;
                }
            }
        }
    }

    cout << "finished with best k : " << K + 1<< endl;
    fout << "finished with best k : " << K + 1<< endl;
    for (auto &r : BestSol) fout << " " << r;
    fout << endl;

    cin.rdbuf(backup);
    return 0;
}

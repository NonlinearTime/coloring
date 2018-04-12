//
// Created by haimin on 4/9/18.
//

#ifndef COLORING_NETFLOW_H
#define COLORING_NETFLOW_H

#include <iostream>
#include <vector>
#include <cstring>
#include <queue>

using namespace std;

struct Edge {
    int s, t, f, c;
    int next;
};

struct edge {
    int u, v, w;
    edge (int u, int v, int w) {this->u = u, this->v = v, this->w = w;}
};

class NetFlow {
private:
    int raw_index_num, index_num, edge_num;
    size_t maxflow = 0;
    int cnt;
    int sumFlow;
    int* pre, *dist, *eat;
    int* inq, *vt;
    Edge *e;

    int32_t inf = 100100100;

public:
    explicit NetFlow(int grapSize);
    void init();
    void addedge(int u, int v, int f, int c);
    bool SPFA(int s, int t, int n);
    int MCMF(int s, int t, int n);

    int getSolution();
    void init(vector<edge > edges);

    ~NetFlow();
    //void clear();
};


#endif //COLORING_NETFLOW_H

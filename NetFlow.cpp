//
// Created by haimin on 4/9/18.
//

#include "NetFlow.h"


NetFlow::NetFlow(int index_num) {
    this->raw_index_num = index_num;
    this->index_num = 2 * index_num + 3;
    edge_num = index_num * index_num + 3 * index_num + 1;
    pre = new int[this->index_num + 100];
    dist = new int[this->index_num + 100];
    eat = new int[this->index_num + 100];
    inq = new int[this->index_num + 100];
    vt = new int[this->index_num + 100];
    e = new Edge[edge_num * 2 + 1000];
}

void NetFlow::init() {
    cnt = 0;
    for (size_t i = 0 ; i < index_num ; ++i) pre[i] = -1;
}

void NetFlow::addedge(int s, int t, int f, int c) {
    e[cnt].s = s, e[cnt].t = t, e[cnt].f = f, e[cnt].c = c, e[cnt].next = pre[s];
    pre[s] = cnt++;
    e[cnt].s = t, e[cnt].t = s, e[cnt].f = 0, e[cnt].c = -c, e[cnt].next = pre[t];
    pre[t] = cnt++;
}

bool NetFlow::SPFA(int s, int t, int n) {
    queue<int > q;
    for (size_t i = 0 ; i < index_num; ++i) {
        inq[i] = 0;
        vt[i] = 0;
        eat[i] = -1;
        dist[i] = inf;
    }
    inq[s] = 1;
    vt[s]++;
    dist[s] = 0;
    q.push(s);
    while(!q.empty()){
        int u = q.front();
        q.pop();
        inq[u] = 0;
        if(vt[u] > n) break;
        for(int i = pre[u]; i != -1; i = e[i].next){
            int v = e[i].t;
            if(e[i].f && dist[v] > dist[u] + e[i].c){
                dist[v] = dist[u] + e[i].c;
                eat[v] = i;
                if(!inq[v]){
                    q.push(v);
                    vt[v]++;
                    inq[v] = 1;
                }
            }
        }
    }
    return !(dist[t] == inf || !q.empty());

}

int NetFlow::MCMF(int s, int t, int n) {
    int flow = 0; // 总流量
    int i, minflow, mincost;
    mincost = 0;
    while(SPFA(s, t, n)){
        minflow = inf + 1;
        for(i = eat[t]; i != -1; i = eat[e[i].s])
            if(e[i].f < minflow) minflow = e[i].f;
        flow += minflow;
        for(i = eat[t]; i != -1; i = eat[e[i].s]){
            e[i].f -= minflow;
            e[i^1].f += minflow;
        }
        mincost += dist[t] * minflow; //单位流量的最小代价dist[t]
    }
    sumFlow = flow; // 题目需要流量，用于判断

    return sumFlow;
}

int NetFlow::getSolution() {
    int res = MCMF(0, index_num - 1, index_num - 1);
    return res;
}

void NetFlow::init(vector<edge> edges) {
    cnt = 0;
    for (size_t i = 0 ; i < index_num ; ++i) pre[i] = -1;
    int src = 0, dest = index_num - 1;
    for(int i = 1; i <= raw_index_num; i ++){
        addedge(src, i, 1, 0);
        addedge(i + raw_index_num, dest, 1, 0);
        addedge(i, index_num - 2, 1, 0);
    }
    for (auto &r: edges) addedge(r.u, r.v + raw_index_num,1,r.w);
    addedge(index_num - 2, index_num - 1, inf, 0);
}

NetFlow::~NetFlow() {
    delete pre;
    delete dist;
    delete eat;
    delete inq;
    delete vt;
    delete[] e;
}

#pragma once

#include <vector>
#include <utility>
#include <set>

double distance(int v, int u);

double g(const std::vector<int>& path)
{
    double cost = 0;
    int n = path.size();
    int u, v;
    if (n > 0) u = path[0];
    for (int i = 1; i < n; ++i) {
        v = path[i];
        cost += distance(v,u);
        u = path[i];
    }
    return cost;
}
/*int h(int v, int u) 
{
    int y1 = v / NUM_COLS, x1 = v % NUM_COLS;
    int y2 = u / NUM_COLS, x2 = u % NUM_COLS;
    return abs(y2-y1)+abs(x2-x1);
}*/
int f(int v, int end, std::vector<int> path)
{
    return g(path) + distance(v, end);
}

std::vector<int> AStar(std::vector<std::vector<std::pair<int, double> > > adj, int s, int e){
    int n = adj.size();
    const int INF = 1e9;
    std::set<std::pair<int,std::vector<int>>> q;
    std::vector<double> value(n, INF);
    q.insert({0, {s}});
    while (!q.empty()) {
        std::vector<int> path = q.begin()->second;
        int v = path.back();
        if (v == e) return path;
        q.erase(q.begin());

        for (auto edge : adj[v]) {
            auto to = edge.first;
            std::vector<int> v = path;
            v.push_back(to);
            double cost = f(to, e, v);
            if (cost <= value[to]) {
                value[to] = cost;
                q.insert({cost,v});
            }
        }
    }
    return {};
}


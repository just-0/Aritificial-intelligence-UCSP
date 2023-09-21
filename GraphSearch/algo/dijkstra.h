#include <vector>
#include <utility>
#include <set>

std::vector<int>
dijkstra(std::vector<std::vector<std::pair<int, double> > > adj,
    int s, int e)
{
    int n = adj.size();
    const double INF = 1e18;
    std::vector<double> d(n, INF);
    std::vector<int> p(n, -1);

    d[s] = 0;
    std::set<std::pair<double, int> > q;
    q.insert({0, s});
    while (!q.empty()) {
        int v = q.begin()->second;
        q.erase(q.begin());

        for (auto &edge : adj[v]) {
            auto to = edge.first;
            auto len = edge.second;

            if (d[v] + len < d[to]) {
                q.erase({d[to], to});
                d[to] = d[v] + len;
                p[to] = v;
                q.insert({d[to], to});
            }
        }
    }

    std::vector<int> route;
    route.push_back(e);
    while (p[e] != -1) {
        e = p[e];
        route.push_back(e);
    }
    
    return route;
}

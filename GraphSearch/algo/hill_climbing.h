#pragma once

#include <vector>
#include <utility>
#include <stack>

double distance(int v, int u);

struct pdist
{
    int n;
    double dist;
};

bool cmp(const pdist &a, const pdist &b)
{
    return a.dist > b.dist;
}

std::vector<pdist> getNeighbours(int n, int end, std::vector<std::vector<std::pair<int, double> > > adj)
{
    std::vector<pdist> res;
    for (int i = 0; i < adj[n].size(); i++)
    {
        pdist tmp;
        tmp.n = adj[n][i].first;
        tmp.dist = distance(tmp.n, end);
        res.push_back(tmp);
    }
    std::sort(res.begin(), res.end(), cmp);
    return res;
}

std::vector<int> hillClimbing(std::vector<std::vector<std::pair<int, double> > > adj, int s, int e)
{
    int n = adj.size();
    std::stack<int> stk;
    int current;
    std::vector<char> visited(n);

    std::vector<pdist> tmp = getNeighbours(s, e, adj);
    std::vector<int> path, parent(n, -1);

    visited[s] = 1;
    /*for (int i = 0; i < tmp.size(); i++)
    {
        stk.push(tmp[i].n);
        visited[tmp[i].n] = 1;
    }*/
    stk.push(s);

    while (!stk.empty())
    {
        current = stk.top();
        if (current == e) {
            break;
        }
        stk.pop();

        std::vector<pdist> tmp2 = getNeighbours(current, e, adj);
        for (int i = 0; i < tmp2.size(); i++)
        {
            if (!visited[tmp2[i].n])
            {
                stk.push(tmp2[i].n);
                parent[tmp2[i].n] = current;
                visited[tmp2[i].n] = 1;
            }
        }

    }

    path.push_back(e);
    while(parent[e] != -1)
    {
        e = parent[e];
        path.push_back(e);
    }
    return path;
}

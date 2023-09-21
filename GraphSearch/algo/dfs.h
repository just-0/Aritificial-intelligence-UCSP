#include <vector>
#include <utility>
#include <list>

std::vector<int>
dfs(std::vector<std::vector<std::pair<int, double>>> adj,
    int start, int end)
{
    int n = adj.size();
    std::vector<char> visited(n);
    std::list<int> queue;

    visited[start] = 1;
    queue.push_back(start);
    
    int current;
    std::vector<int> path, parent(n, -1);
    while (!queue.empty())
    {
        current = queue.front();
        //path.push_back(current);

        if (current == end)
        {
            break;
        }
        queue.pop_front();

        for (size_t i = 0; i < adj[current].size(); i++)
        {
            if (!visited[adj[current][i].first])
            {
                visited[adj[current][i].first] = 1;
                parent[adj[current][i].first] = current;
                queue.push_front(adj[current][i].first);
            }
        }
    }
    
    path.push_back(end);
    while(parent[end] != -1)
    {
        end = parent[end];
        path.push_back(end);
    }
    
    return path;
}

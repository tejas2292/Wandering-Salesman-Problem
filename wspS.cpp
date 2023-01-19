#include <iostream>
#include <cstring>
#include <algorithm>
using namespace std;

const int N = 20;
int n;
int dummy = 1000;
int dist[N][N];
int curr_path[N];
int best_path[N];
int curr_cost;
int best_cost;
bool visited[N];

void read()
{
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cin >> dist[i][j];
        }
    }
}

void init()
{
    memset(curr_path, -1, sizeof curr_path);
    memset(best_path, -1, sizeof best_path);
    memset(visited, false, sizeof visited);
    curr_cost = 0;
    best_cost = 1e9;
}

void copy_path()
{
    for (int i = 0; i < n; i++)
    {
        best_path[i] = curr_path[i];
    }
}

int bound(int u)
{
    int curr_bound = curr_cost;

    for (int i = 0; i < n; i++)
    {
        if (!visited[i])
        {
            curr_bound += dist[u][i];
        }
    }

    return curr_bound;
}

void tsp(int u, int level)
{
    if (level == n)
    {
        if (dist[u][0] != -1)
        {
            curr_cost += dummy;
            if (curr_cost < best_cost)
            {
                best_cost = curr_cost;
                copy_path();
            }
            curr_cost -= dummy;
        }
        return;
    }

    for (int v = 0; v < n; v++)
    {
        if (dist[u][v] != -1 && !visited[v])
        {
            if (curr_cost + dist[u][v] + bound(v) < best_cost)
            {
                curr_path[level] = v;
                visited[v] = true;
                curr_cost += dist[u][v];
                tsp(v, level + 1);
                curr_cost -= dist[u][v];
                visited[v] = false;
            }
        }
    }
}

int main()
{
    read();
    init();

    curr_path[0] = 0;
    visited[0] = true;
    tsp(0, 1);

    cout << "Minimum cost: " << best_cost - dummy << endl;
    cout << "Path: ";
    for (int i = 0; i < n; i++)
    {
        cout << best_path[i] + 1 << " ";
    }

    return 0;
}
#include <bits/stdc++.h>
#include <stdio.h>
#include <cstring>
#include <algorithm>

const int N = 100;
int n;
int dist[N][N];
int curr_path[N];
int best_path[N];
int curr_bound;
int bound;
bool visited[N];

void copy_path()
{
    for (int i = 0; i < n; i++)
    {
        best_path[i] = curr_path[i];
    }
}

void tsp(int u, int level)
{
    if (level == n)
    {
        if (curr_bound < bound)
        {
            bound = curr_bound;
            copy_path();
        }

        return;
    }

    for (int v = 0; v < n; v++)
    {
        if (dist[u][v] != -1 && !visited[v])
        {
            int temp_bound = curr_bound + dist[u][v];
            if (temp_bound < bound)
            {
                curr_path[level] = v;
                visited[v] = true;
                curr_bound += dist[u][v];
                tsp(v, level + 1);
                curr_bound -= dist[u][v];
                visited[v] = false;
            }
        }
    }
}

int main()
{
    FILE *file = fopen("dist17", "r");

    fscanf(file, "%d", &n);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            if (i == j)
            {
            }
            else
            {
                fscanf(file, "%d", &dist[i][j]);
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (i == j)
            {
            }
            else
            {
                dist[i][j] = dist[j][i];
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        dist[i][i] = 0;
    }

    fclose(file);

    memset(curr_path, -1, sizeof curr_path);
    memset(best_path, -1, sizeof best_path);
    memset(visited, false, sizeof visited);
    curr_bound = 0;
    bound = 1000000;

    srand((unsigned)time(NULL));
    curr_path[0] = 0;
    visited[0] = true;
    tsp(0, 1);

    printf("Cost: %d\n", bound);
    printf("Path: \n");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", best_path[i]);
    }

    return 0;
}
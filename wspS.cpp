#include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <time.h>
using namespace std;

const int N = 100;
int n;
int dist[N][N];
int curr_path[N];
int best_path[N];
int curr_bound;
int bound;
bool visited[N];

void read()
{
    FILE *file = fopen("dist4", "r");

    /* Read the number of nodes */
    fscanf(file, "%d", &n);

    /* Read the lower triangular matrix */
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            if (i != j)
            {
                fscanf(file, "%d", &dist[i][j]);
            }
        }
    }

    /* Replicate the upper triangular matrix */
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (i != j)
            {
                dist[i][j] = dist[j][i];
            }
        }
    }

    /* Set diagonal elements to 0 */
    for (int i = 0; i < n; i++)
    {
        dist[i][i] = 0;
    }

    /* Print the distance matrix */
    printf("The number of cities : %d\n", n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", dist[i][j]);
        }
        printf("\n");
    }

    /* Close the input file */
    fclose(file);
}

void init()
{
    memset(curr_path, -1, sizeof curr_path);
    memset(best_path, -1, sizeof best_path);
    memset(visited, false, sizeof visited);
    curr_bound = 0;
    bound = 1e9;
}

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
    read();
    init();
    clock_t startTime, endTime;
    startTime = clock();
    srand((unsigned)time(NULL));
    curr_path[0] = 0;
    visited[0] = true;
    tsp(0, 1);
    endTime = clock();

    cout << "Minimum cost: " << bound << endl;
    cout << "Path: ";
    for (int i = 0; i < n; i++)
    {
        cout << best_path[i] << " ";
    }
    printf("\nThe computation took %.2lf seconds\n", (double)(endTime - startTime) / CLOCKS_PER_SEC);

    return 0;
}
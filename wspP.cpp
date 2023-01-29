#include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <time.h>
#include <cmath>
#include <mpi.h>
using namespace std;

const int N = 100;
int n;
int dist[N][N];
int curr_path[N];
int best_path[N];
int curr_bound;
int bound;
bool visited[N];
int ctr1 = 0;

void read()
{
    FILE *file = fopen("dist17", "r");

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
    bound = 0;
}

void copy_path()
{
    for (int i = 0; i < n; i++)
    {
        best_path[i] = curr_path[i];
    }
}

void tsp(int u, int level, int task_id)
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
        if (level == 2 && v == 0)
        {
            ctr1 = u;
            while (ctr1 != 0)
            {
                if (ctr1 - 1 != 0)
                {
                    visited[ctr1 - 1] = false;
                }
                ctr1--;
            }
        }

        if (dist[u][v] != -1 && !visited[v])
        {
            int temp_bound = curr_bound + dist[u][v];
            if (temp_bound < bound)
            {
                curr_path[level] = v;
                visited[v] = true;
                curr_bound += dist[u][v];
                tsp(v, level + 1, task_id);
                curr_bound -= dist[u][v];
                visited[v] = false;
                if (level == 1)
                {
                    return;
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    int start, end;
    int temp_bound1 = 10000;
    int temp_bound2;
    int temp_path1[N];
    int temp_path2[N];

    int temp = 10000;
    int temp_p[N];
    /* Initialize MPI */
    MPI_Init(&argc, &argv);
    /* Get the number of tasks */
    double startTime;
    int num_tasks;
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

    /* Get the task ID */
    int task_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);

    /* Master process */
    if (task_id == 0)
    {
        /* Read the input */
        read();

        /* Initialize variables */
        init();

        int u = 0;
        for (int i = 0; i < n; i++)
        {
            if (dist[u][i] != 0)
            {
                bound += dist[u][i];
                u++;
            }
        }
    }

    memset(curr_path, -1, sizeof curr_path);
    memset(best_path, -1, sizeof best_path);
    memset(visited, false, sizeof visited);
    curr_bound = 0;
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&dist, N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&bound, 1, MPI_INT, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();

    int remaining_cities = n % num_tasks;
    int block_size;
    if (n >= num_tasks)
    {
        block_size = ceil(n / num_tasks);
        start = task_id * block_size;
        end = start + block_size - 1;
    }
    else
    {
        block_size = 1;
        start = task_id;
        end = start;
    }
    if (task_id < remaining_cities)
    {
        start += task_id;
        end = start + block_size;
    }
    else
    {
        start += remaining_cities;
        end = start + block_size - 1;
    }

    for (int i = start; i <= end; i++)
    {
        // if(task_id != 0){
        for (int k = 0; k <= i; k++)
        {
            visited[k] = true;
        }
        curr_path[0] = 0;
        tsp(0, 1, task_id);

        if (bound < temp_bound1)
        {
            temp_bound1 = bound;
            for (int i = 0; i < n; i++)
            {
                temp_path1[i] = best_path[i];
            }
        }
    }

    /* Send the best path and bound to the master */
    MPI_Send(&temp_bound1, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(temp_path1, N, MPI_INT, 0, 3, MPI_COMM_WORLD);

    /*
     cout << "Minimum cost: " << bound <<" BY rank : "<<task_id<<endl;
     cout << "Path: ";
     for (int i = 0; i < n; i++)
     {
         cout << best_path[i] << " ";
     }
     cout<<" By rank : "<<task_id;
     */
    /* Slave processes */

    /* Receive the values */

    /* Slave process handles its portion of the tree */

    // for (int i = start; i <= task_id; i++) {
    //   visited[i] = true;
    // }
    // tsp(0, 1, task_id);

    //}
    /* Finalize MPI */
    // MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    if (task_id == 0)
    {
        /* Receive the best path and bound from the slaves */
        for (int i = 1; i < num_tasks; i++)
        {
            MPI_Recv(&temp_bound2, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(temp_path2, N, MPI_INT, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            /* Update the best path and bound if necessary */
            if (temp_bound2 < temp_bound1)
            {
                temp_bound1 = temp_bound2;
                for (int j = 0; j < n; j++)
                {
                    temp_path1[j] = temp_path2[j];
                }
            }
        }

        /* Print the best path and bound */
        cout << "Minimum cost: " << temp_bound1 << endl;
        cout << "Path: ";
        for (int i = 0; i < n; i++)
        {
            cout << temp_path1[i] << " ";
        }
        printf("\nThe computation took %.2lf seconds\n", elapsed_time);
    }
    MPI_Finalize();

    return 0;
}

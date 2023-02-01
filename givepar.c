#include <stdio.h>
#include <stdbool.h>
#include <mpi.h>
#include <string.h>
#include <math.h>

const int N = 100;
int n;
int dist[100][100];
int curr_path[100];
int best_path[100];
int curr_bound;
int bound;
bool visited[100];
int ctr1 = 0;

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
        if (curr_bound <= bound)
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
    MPI_Init(&argc, &argv);
    int num_tasks;
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

    int task_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);

    if (task_id == 0)
    {
        FILE *file = fopen("dist4", "r");

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

        curr_path[0] = 0;
        visited[0] = true;
        tsp(0, 1, task_id);
    }

    memset(curr_path, -1, sizeof curr_path);
    memset(best_path, -1, sizeof best_path);
    memset(visited, false, sizeof visited);
    curr_bound = 0;
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&dist, N * N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&bound, 1, MPI_INT, 0, MPI_COMM_WORLD);

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

    MPI_Send(&temp_bound1, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(temp_path1, N, MPI_INT, 0, 3, MPI_COMM_WORLD);

    if (task_id == 0)
    {
        int temp = 10000;
        for (int i = 1; i < num_tasks; i++)
        {
            MPI_Recv(&temp_bound2, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(temp_path2, N, MPI_INT, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (temp_bound2 < temp)
            {
                temp = temp_bound2;
                for (int j = 0; j < n; j++)
                {
                    temp_path1[j] = temp_path2[j];
                }
            }
        }

        printf("Cost: %d\n", temp);
        printf("Path: \n");
        for (int i = 0; i < n; i++)
        {
            printf("%d ", temp_path1[i]);
        }
    }
    MPI_Finalize();

    return 0;
}
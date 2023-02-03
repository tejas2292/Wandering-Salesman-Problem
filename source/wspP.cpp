#include <bits/stdc++.h>
#include <iostream>
#include <time.h>
#include <cmath>
#include <mpi.h>
using namespace std;

const int MAX_SIZE = 100;
int n;
int dist[MAX_SIZE][MAX_SIZE];
int current_path[MAX_SIZE];
int best_path[MAX_SIZE];
int current_bound;
int bound;
bool visited_cities[MAX_SIZE];
int counter = 0;
int temp_bound1 = 10000;
int temp_bound2;
int temp_path1[MAX_SIZE];
int temp_path2[MAX_SIZE];
int temp_p[MAX_SIZE];
string fileName;
int divide_size;
int available_cities;
int start_rank, end_rank;

void readFile(string &fileName);
void initialization();
void copy_path();
void calculateInitialBound();
void divideAndConquer(int size, int rank);
void WSP(int curr_city, int level, int rank);

int main(int argc, char *argv[])
{
    int size, rank;
    double start_time, end_time, elapsed_time;
    double time1, time2, time3, time4, time5, time6;

    /* Initialize MPI */
    MPI_Init(&argc, &argv);

    /* Get the size of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Get the rank of process */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Master process */
    if (rank == 0)
    {
        /* check user has given file name or not */
        if (argc < 2)
        {
            cerr << "Please provide a file name as command line argument." << endl;
            exit(1);
        }
    }
    else // in remaining processes also check file is passed or not
    {
        /* check user has given file name or not */
        if (argc < 2)
        {
            exit(1);
        }
    }
    if (rank == 0)
    {
        /* call function to read the file */
        readFile(fileName.assign(argv[1]));

        start_time = MPI_Wtime();

        /* initialize the required memory to the variables */
        initialization();

        /* Calculate the bound */
        calculateInitialBound();
    }

    /* All rocesses including 0 as well */
    memset(current_path, -1, sizeof current_path);
    memset(best_path, -1, sizeof best_path);
    memset(visited_cities, false, sizeof visited_cities);
    current_bound = 0;
    time1 = MPI_Wtime();
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&dist, MAX_SIZE * MAX_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&bound, 1, MPI_INT, 0, MPI_COMM_WORLD);
    time2 = MPI_Wtime();
    /* Divide the cities among the processes */
    divideAndConquer(size, rank);

    /* Every process handles its portion of the tree */
    if (start_rank != -1) // check weather the processor is having allocated cities
    {
        for (int i = start_rank; i <= end_rank; i++)
        {
            if (i != 0 && dist[0][i] != 0)
            {
                current_path[0] = 0; // { save the path of level 1,
                current_path[1] = i; // level 2, cities as we are not travering the path from level 1 }
                visited_cities[0] = true;
                visited_cities[i] = true;
                current_bound += dist[0][i]; // pre-calculate distance of level 1 city 0 to current city of level 2 i.e new leaf node
                WSP(i, 2, rank);             // consider leaf node at level 2
                current_bound = 0;
                memset(visited_cities, false, sizeof visited_cities);
            }
            if (bound < temp_bound1)
            {
                temp_bound1 = bound;
                for (int i = 0; i < n; i++)
                {
                    temp_path1[i] = best_path[i];
                }
            }
        }
    }
    /* Send the best path and bound to the master */
    time3 = MPI_Wtime();
    MPI_Send(&temp_bound1, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(temp_path1, MAX_SIZE, MPI_INT, 0, 3, MPI_COMM_WORLD);
    time4 = MPI_Wtime();

    /* Master process final comparison by gathering information from all processes and printing output */
    if (rank == 0)
    {
        /* Receive the best path and bound from the slaves */
        for (int i = 1; i < size; i++)
        {
            time5 = MPI_Wtime();
            MPI_Recv(&temp_bound2, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(temp_path2, MAX_SIZE, MPI_INT, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            time6 = MPI_Wtime();

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

        end_time = MPI_Wtime();
        elapsed_time = end_time - start_time;

        /* Print the best path and bound */
        cout << "Minimum cost: " << temp_bound1 << endl;
        cout << "Path: ";
        for (int i = 0; i < n; i++)
        {
            cout << temp_path1[i] << " ";
        }
        printf("\nThe Program Took %.5lf Seconds\n", elapsed_time);
        double costOfCommunication = (time2 - time1) + (time4 - time3) + (time6 - time5);
        printf("The Cost of Computation Took %.5lf Seconds\n", elapsed_time - costOfCommunication);
        printf("The Cost of Communication Took %.5lf Seconds\n", costOfCommunication);
    }

    /* Finalize MPI */
    MPI_Finalize();

    return 0;
}

void readFile(string &fileName)
{
    FILE *file = fopen(fileName.c_str(), "r");

    /* Check given file is present or not */
    if (file == NULL)
    {
        printf("Error: file %s does not exist.\n", fileName.c_str());
        exit(1);
    }

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

void initialization()
{
    memset(current_path, -1, sizeof current_path);        // set current path as -1
    memset(best_path, -1, sizeof best_path);              // set best path as -1
    memset(visited_cities, false, sizeof visited_cities); // set all cities as visited false
    current_bound = 0;                                    // set currrent bound as 0
    bound = 0;                                            // set bound as 0
}

void copy_path()
{
    for (int i = 0; i < n; i++)
    {
        best_path[i] = current_path[i]; // copy all the contents of current path to best path
    }
}

void calculateInitialBound()
{
    /* start the city as u from 0 and increase it by 1 until we reach our destination. */
    /* (city 0 (u) -> city 1 (i), city 1 (u) -> city 2 (i)) */
    int u = 0;
    for (int i = 0; i < n; i++)
    {
        if (dist[u][i] != 0)
        {
            bound += dist[u][i]; // store the result into bound variable
            u++;
        }
    }
}

void divideAndConquer(int size, int rank)
{
    /* this function is for dividing total number of cities into processors */
    /* e.g (4 city 4 processor, process 0 -> city 0, process 1 -> city 1) or (4 city 2 processor, process 0 -> city 0 & 1, process 1 -> city 2 & 3)*/
    if (rank < n)
    {
        available_cities = n % size;
        if (n >= size)
        {
            divide_size = ceil(n / size);
            start_rank = rank * divide_size;
            end_rank = start_rank + divide_size - 1;
        }
        else
        {
            divide_size = 1;
            start_rank = rank;
            end_rank = start_rank;
        }
        if (rank < available_cities)
        {
            start_rank += rank;
            end_rank = start_rank + divide_size;
        }
        else
        {
            start_rank += available_cities;
            end_rank = start_rank + divide_size - 1;
        }
    }
    else // if the rank of processor is greater than total cities then dont allocate them any cities i.e extra processors
    {
        start_rank = -1;
        end_rank = -1;
    }
}

void WSP(int curr_city, int level, int rank)
{
    if (level == n) // check whether level is final or not
    {
        if (current_bound < bound) // check whether current bound is less than global bound
        {
            bound = current_bound; // change global bound value to new minimum bound
            copy_path();           // set this new bound path as best path
        }

        return;
    }

    for (int v = 0; v < n; v++)
    {
        /* checking whether next visiting city is present and yet not visited */
        if (dist[curr_city][v] != -1 && !visited_cities[v])
        {
            int temp_bound = current_bound + dist[curr_city][v];
            if (temp_bound < bound) // prune the path if upcoming bound is greater than global bound
            {
                current_path[level] = v;
                visited_cities[v] = true;
                current_bound += dist[curr_city][v];
                WSP(v, level + 1, rank); // recusrion call for next city at next level

                /* checking other root at previous level */
                current_bound -= dist[curr_city][v];
                visited_cities[v] = false;
            }
        }
    }
}
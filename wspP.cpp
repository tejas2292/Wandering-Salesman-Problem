#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <limits.h>

#define MAX_NODES 100 // Maximum number of cities can be 100
#define DUMMY 1000    // To keep bound in limit

/* Function prototypes */
void wanderingSalesmanProblem(int num_nodes, int distances[][MAX_NODES], int path[], int path_length, int path_cost, int *best_path, int *best_path_cost);
int calculateBound(int num_nodes, int distances[][MAX_NODES], int path[], int path_length);
void printPath(int num_nodes, int path[], int path_cost);
int readFile(int num_nodes, int distances[][MAX_NODES]);

int main(int argc, char **argv)
{
    int num_nodes;
    int distances[MAX_NODES][MAX_NODES];
    int path[MAX_NODES];
    int best_path[MAX_NODES];
    int path_length, path_cost, best_path_cost;
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Read input from file */
    if (rank == 0)
    {
        num_nodes = readFile(num_nodes, distances);
    }

    /* Broadcast the number of nodes and distance matrix to all processes */
    MPI_Bcast(&num_nodes, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(distances, MAX_NODES * MAX_NODES, MPI_INT, 0, MPI_COMM_WORLD);

    /* Initialize path and best_path to -1 */
    for (int i = 0; i < num_nodes; i++)
    {
        path[i] = -1;
        best_path[i] = -1;
    }

    /* Set the initial path to start from node 0 */
    path[0] = 0;
    path_length = 1;
    path_cost = 0;
    best_path_cost = INT_MAX;

    /* Start the branch and bound algorithm */
    wanderingSalesmanProblem(num_nodes, distances, path, path_length, path_cost, best_path, &best_path_cost);

    /* Print the best path and its cost */
    if (rank == 0)
    {
        printf("Best path: ");
        printPath(num_nodes, best_path, best_path_cost);
    }

    MPI_Finalize();
    return 0;
}

void wanderingSalesmanProblem(int num_nodes, int distances[][MAX_NODES], int path[], int path_length, int path_cost, int *best_path, int *best_path_cost)
{
    int current_bound;
    int current_path[MAX_NODES];

    /* Calculate the current bound */
    current_bound = calculateBound(num_nodes, distances, path, path_length);

    /* If the current bound is greater than or equal to the best path cost, return */
    if (current_bound >= *best_path_cost)
    {
        return;
    }

    /* If we have reached the last node, check if the path cost is better than the current best path */
    if (path_length == num_nodes)
    {
        /* Add the cost of the edge from the last node to the starting node */
        path_cost += DUMMY;

        /* Update the best path and best path cost if this path is better */
        if (path_cost < *best_path_cost)
        {
            *best_path_cost = path_cost;
            for (int i = 0; i < num_nodes; i++)
            {
                best_path[i] = path[i];
            }
        }
        path_cost -= DUMMY;
        return;
    }

    /* Copy the current path to a new array */
    for (int i = 0; i < num_nodes; i++)
    {
        current_path[i] = path[i];
    }

    /* Try all possible next nodes for the current path */
    for (int i = 0; i < num_nodes; i++)
    {
        /* Check if the node has already been visited */
        int visited = 0;
        for (int j = 0; j < path_length; j++)
        {
            if (path[j] == i)
            {
                visited = 1;
                break;
            }
        }
        if (visited)
        {
            continue;
        }

        /* Add the node to the path and update the path cost */
        current_path[path_length] = i;
        path_cost += distances[path[path_length - 1]][i];

        /* Recursively call the function with the updated path */
        wanderingSalesmanProblem(num_nodes, distances, current_path, path_length + 1, path_cost, best_path, best_path_cost);

        /* Remove the node from the path and revert the path cost */
        current_path[path_length] = -1;
        path_cost -= distances[path[path_length - 1]][i];
    }
}

int calculateBound(int num_nodes, int distances[][MAX_NODES], int path[], int path_length)
{
    int bound = 0;
    int min_distance;
    int visited[num_nodes];

    /* Initialize visited array to 0 */
    for (int i = 0; i < num_nodes; i++)
    {
        visited[i] = 0;
    }

    /* Mark all visited nodes as 1 */
    for (int i = 0; i < path_length; i++)
    {
        visited[path[i]] = 1;
    }

    /* For each unvisited node, find the minimum distance to a visited node */
    for (int i = 0; i < num_nodes; i++)
    {
        if (visited[i] == 1)
        {
            continue;
        }
        min_distance = INT_MAX;
        for (int j = 0; j < path_length; j++)
        {
            if (distances[i][path[j]] < min_distance)
            {
                min_distance = distances[i][path[j]];
            }
        }

        /* Add the minimum distance to the bound */
        bound += min_distance;
    }

    /* Add the distance from the last visited node to the starting node */
    bound += distances[path[path_length - 1]][path[0]];

    return bound;
}

void printPath(int num_nodes, int path[], int path_cost)
{
    for (int i = 0; i < num_nodes; i++)
    {
        printf("%d ", path[i]);
    }
    printf("\nPath Cost: %d\n", path_cost - DUMMY);
}

int readFile(int num_nodes, int distances[][MAX_NODES])
{
    /* Open the input file */
    FILE *file = fopen("dist4", "r");

    /* Read the number of nodes */
    fscanf(file, "%d", &num_nodes);
    printf("Num Nodes : %d\n", num_nodes);
    /* Read the lower triangular matrix */
    for (int i = 0; i < num_nodes; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            if (i != j)
            {
                fscanf(file, "%d", &distances[i][j]);
            }
        }
    }

    /* Replicate the upper triangular matrix */
    for (int i = 0; i < num_nodes; i++)
    {
        for (int j = i + 1; j < num_nodes; j++)
        {
            if (i != j)
            {
                distances[i][j] = distances[j][i];
            }
            else
            {
            }
        }
    }

    /* Set diagonal elements to 0 */
    for (int i = 0; i < num_nodes; i++)
    {
        distances[i][i] = 0;
    }

    /* Print the distance matrix */
    for (int i = 0; i < num_nodes; i++)
    {
        for (int j = 0; j < num_nodes; j++)
        {
            printf("%d ", distances[i][j]);
        }
        printf("\n");
    }

    /* Close the input file */
    fclose(file);
    return num_nodes;
}
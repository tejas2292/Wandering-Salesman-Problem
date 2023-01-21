#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <limits.h>

#define MAX_NODES 100
#define dummy 1000

/* Function prototypes */
void branch_and_bound(int num_nodes, int distances[][MAX_NODES], int path[], int path_length, int path_cost, int *best_path, int *best_path_cost);
int calculate_bound(int num_nodes, int distances[][MAX_NODES], int path[], int path_length);
void print_path(int num_nodes, int path[], int path_cost);

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
        printf("Enter the number of nodes: ");
        scanf("%d", &num_nodes);

        printf("Enter the distance matrix: \n");
        for (int i = 0; i < num_nodes; i++)
        {
            for (int j = 0; j < num_nodes; j++)
            {
                scanf("%d", &distances[i][j]);
            }
        }
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
    branch_and_bound(num_nodes, distances, path, path_length, path_cost, best_path, &best_path_cost);

    /* Print the best path and its cost */
    if (rank == 0)
    {
        printf("Best path: ");
        print_path(num_nodes, best_path, best_path_cost);
    }

    MPI_Finalize();
    return 0;
}

void branch_and_bound(int num_nodes, int distances[][MAX_NODES], int path[], int path_length, int path_cost, int *best_path, int *best_path_cost)
{
    int current_bound;
    int current_path[MAX_NODES];

    /* Calculate the current bound */
    current_bound = calculate_bound(num_nodes, distances, path, path_length);

    /* If the current bound is greater than or equal to the best path cost, return */
    if (current_bound >= *best_path_cost)
    {
        return;
    }

    /* If we have reached the last node, check if the path cost is better than the current best path */
    if (path_length == num_nodes)
    {
        /* Add the cost of the edge from the last node to the starting node */
        path_cost += dummy;

        /* Update the best path and best path cost if this path is better */
        if (path_cost < *best_path_cost)
        {
            *best_path_cost = path_cost;
            for (int i = 0; i < num_nodes; i++)
            {
                best_path[i] = path[i];
            }
        }
        path_cost -= dummy;
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
        branch_and_bound(num_nodes, distances, current_path, path_length + 1, path_cost, best_path, best_path_cost);

        /* Remove the node from the path and revert the path cost */
        current_path[path_length] = -1;
        path_cost -= distances[path[path_length - 1]][i];
    }
}

int calculate_bound(int num_nodes, int distances[][MAX_NODES], int path[], int path_length)
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

void print_path(int num_nodes, int path[], int path_cost)
{
    for (int i = 0; i < num_nodes; i++)
    {
        printf("%d ", path[i]);
    }
    printf("\nPath cost: %d\n", path_cost - dummy);
}
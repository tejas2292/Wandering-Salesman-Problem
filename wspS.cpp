#include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <time.h>
using namespace std;

const int MAX_SIZE = 100;
int n;
int dist[MAX_SIZE][MAX_SIZE];
int current_path[MAX_SIZE];
int best_path[MAX_SIZE];
int current_bound;
int bound;
bool visited_cities[MAX_SIZE];

void readFile(const string &fileName);
void initialization();
void copy_path();
void WSP(int curr_city, int level);

int main(int argc, char *argv[])
{
    /* check user has given file name or not */
    if (argc < 2)
    {
        cerr << "Please provide a file name as command line argument." << endl;
        return 1;
    }

    string fileName(argv[1]);

    /* call function to read the file */
    readFile(fileName);

    /* initialize the required memory to the variables */
    initialization();

    /* use clock_t function to measure time of program */
    clock_t startTime, endTime;
    startTime = clock();
    srand((unsigned)time(NULL));

    current_path[0] = 0;      // start the program from node 0
    visited_cities[0] = true; // city 0 will be true as we are starting from city 0
    WSP(0, 1);                // pass current city as 0 and level 1

    endTime = clock();

    /* Print the output of the program */
    cout << "Minimum cost: " << bound << endl;
    cout << "Path: ";
    for (int i = 0; i < n; i++)
    {
        cout << best_path[i] << " ";
    }
    printf("\nThe computation took %.2lf seconds\n", (double)(endTime - startTime) / CLOCKS_PER_SEC);

    return 0;
}

void readFile(const string &fileName)
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
    bound = 1e9;                                          // set bound as largest value 1,000,000,000
}

void copy_path()
{
    for (int i = 0; i < n; i++)
    {
        best_path[i] = current_path[i]; // copy all the contents of current path to best path
    }
}

void WSP(int curr_city, int level)
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
                WSP(v, level + 1); // recusrion call for next city at next level

                /* checking other root at previous level */
                current_bound -= dist[curr_city][v];
                visited_cities[v] = false;
            }
        }
    }
}
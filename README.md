# Wandering-Salesman-Problem

## Serial Algorithm
The function WSP takes in two parameters: curr_city which represents the current city the salesman is in and level which represents the current depth of the search tree. The function first checks if the level is equal to n (the total number of cities) and if so, it compares the current bound with the global bound. If the current bound is lower than the global bound, it updates the global bound and copies the current path as the best path. Next, the function loops through all the cities and checks if the next city is present and not yet visited. If so, it calculates the temporary bound by adding the distance between the current city and the next city. If the temporary bound is lower than the global bound, it updates the current path, marks the next city as visited, adds the distance to the current bound, and calls the WSP function recursively with the next city and the next level and if not, the program will prune the tree and won’t travel further on that root. Finally, it resets the current bound and visited status to prepare for checking the next city.

## Parallel Algorithm
All processes carry out the primary task (Master and slaves). The master process first calculates the initial bound after reading the input file and initializing the necessary memory. Then, the master process broadcasts to all other processes n (number of cities), dist (distance matrix), and bound (current bound). The divide and conquer method is then used to divide the tree into manageable chunks for each task. Working on its own branch of the tree, each process determines the optimum path and relays it to the master process. Finally, the master process selects the optimal path and bound among the processes after receiving them from each.

## Project structure

- source/
  - wspP.cpp -> _Parallel Algorithm WSP_
  - wspS.cpp -> _Serial Algorithm WSP_
  - dist17 -> _Input Problem of 17 Distances_

## Getting Started

<ins>Remember to be in project root folder "Wandering-Salesman-Problem/source" before proceeding</ins><br>
Execute the following steps in order to have a fully running program:<br>

### Compilation

1. Compile the file _wspS.cpp_ with the following command:

```
g++ wspS.cpp
```

2. Compile the file _wspP.cpp_ with the following command:

```
mpicxx wspP.cpp
```

### Execution

1. Execute object code file of _wspS.cpp_ with the following command:

```
./a.out [inputFile]
```

Pass the one parameter as a input/distances of cities

2. Execute object code file of _wspP.cpp_ with the following command:

```
mpirun -np [number of cpus] ./a.out [inputFile]
```

Pass the one parameter as a input/distances of cities

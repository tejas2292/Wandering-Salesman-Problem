# Wandering-Salesman-Problem

The matrix multiplication and addition algorithm is included in the project in both sequential and parallel iterations.
This paper compares the parallel and sequential algorithms for matrix multiplication and addition and explains the performance differences and potential advantages (if any) of the parallel solution for cloud architecture employing
parallel computing parameters. Both of the algorithms were created using the C programming language. The Open MPI package was used to create the parallel algorithm. Every type of matrix pair A, B is compatible with the solution (with the requirement that
the number of columns of matrix A must be equal to the number of rows of matrix B). Additionally, the number of rows and/or columns in matrices can be divided by the number of processors or not. Input matrices can be square or
not square. Files are used as input and output in the parallel solution that was built. The output product matrix is written in file "outfile" once the input has been read from file "infile." The algorithm uses the following work division
technique when given p processing units and the input matrices A:m x n and B: n x l

## Project structure

- source/
  - matrixAdditionS.c -> _Sequential algorithm Addition_
  - matrixAdditionP.c -> _Parallel algorithm Addition_
  - matrixMultiplicationS.c -> _Sequential algorithm Multiplication_
  - matrixMultiplicationP.c -> _Parallel algorithm Multiplication_
  - matrixOps.c -> _Matrix operations for creation, write and read_
  - matrixOps.h -> _Header file to include in source codes_
  - writeMatrix.c -> _Writes input matrices A and B into the file "test/infile"_
- test/ -> _Automatically generated folder at first writeMatrix execution_

## Getting Started

<ins>Remember to be in project root folder "MPI-on-Cluster/" before proceeding</ins><br>
Execute the following steps in order to have a fully running program:<br>

### Compilation

1. Compile the file _writeMatrix.c_ with the following command:

```
gcc source/writeMatrix.c source/matrixOps.c -o writeMatrix
```

2. Compile the file _matrixMultiplicationS.c_ with the following command:

```
gcc source/matrixMultiplicationS.c source/matrixOps.c -o seqMul
```

3. Compile the file _matrixMultiplicationP.c_ with the following command:

```
mpicc source/matrixMultiplicationP.c source/matrixOps.c -o parMul
```

4. Compile the file _matrixAdditionS.c_ with the following command:

```
gcc source/matrixAdditionS.c source/matrixOps.c -o seqAdd
```

5. Compile the file _matrixAdditionP.c_ with the following command:

```
mpicc source/matrixAdditionP.c source/matrixOps.c -o parAdd
```

### Execution

<ins>Change directory to "MPI-on-Cluster" before proceeding with the following commands<ins>

1. Execute file _writeMatrix_ with the following command:

```
./writeMatrix [rows A] [columns A] [rows B] [columns B]
```

Pass the four parameters to generate matrices A and B of the desired size

2. At this point you can:
   _ Execute the sequential program with the following command:
   `    ./seqMul or ./seqAdd
   `
   OR
   _ Execute the parallel program on local machine with the following command:
   `    mpirun -np [number of cpus] ./parMul or ./parAdd
   `
   OR \* Execute the parallel program on distributed environment with the following command:
   `    mpirun -np [number of cpus] --hostfile [hfile] ./parMul or ./parAdd
   `
   <br>

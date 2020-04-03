#!/bin/sh
mpicxx -O3 collatz_MPI.cpp -o collatz_mpi

echo "COMPILE SUCCESSFULLY"

sbatch collatz_MPI1.sub
sbatch collatz_MPI12.sub
sbatch collatz_MPI24.sub


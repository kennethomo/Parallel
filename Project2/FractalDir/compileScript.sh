#!/bin/sh
mpicxx -O3 fractal_MPI.cpp -o fractal_mpi

echo "COMPILE SUCCESSFULLY"

#sbatch collatz_MPI1.sub
#sbatch collatz_MPI12.sub
#sbatch collatz_MPI24.sub


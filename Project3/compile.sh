#!/bin/sh
icc -O3 -pthread vectoradd_pthread.cpp -o vectoradd_pthread

echo "COMPILE SUCCESSFULLY"

#sbatch collatz_MPI1.sub
#sbatch collatz_MPI12.sub
#sbatch collatz_MPI24.sub

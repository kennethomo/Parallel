#!/bin/sh
g++ -O3 -pthread fractal_pthread.cpp -o fractal_pthread
#icc -O3 -pthread collatz_pthread.cpp -o collatz_pthread

echo "COMPILE SUCCESSFULLY"

#sbatch collatz_MPI1.sub
#sbatch collatz_MPI12.sub
#sbatch collatz_MPI24.sub

/*
Collatz code for CS 4380 / CS 5351

Copyright (c) 2020 Texas State University. All rights reserved.

Redistribution in source or binary form, with or without modification,
is *not* permitted. Use in source and binary forms, with or without
modification, is only permitted for academic use in CS 4380 or CS 5351
at Texas State University.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Author: Martin Burtscher
*/

#include <cstdio>
#include <algorithm>
#include <sys/time.h>
#include <mpi.h> //added mpi header file

static int collatz(const long bound, int comm_sz, int my_rank)
{
  // compute sequence lengths
  int maxlen = 0;
  for (long i = my_rank + 1; i < bound; i+=comm_sz) {
    long val = i;
    int len = 1;
    while (val != 1) {
      len++;
      if ((val % 2) == 0) {
        val = val / 2;  // even
      } else {
        val = 3 * val + 1;  // odd
      }
    }
    maxlen = std::max(maxlen, len);
  }

  return maxlen;
}

int main(int argc, char *argv[])
{
  int comm_sz, my_rank;
  MPI_Init(NULL, NULL); // initialize MPI
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);//number of processes
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);//proccess rank

  if (my_rank == 0) printf("Collatz v1.3\n");

  // check command line
  if (argc != 2) {fprintf(stderr, "USAGE: %s upper_bound\n", argv[0]); exit(-1);}
  const long bound = atol(argv[1]);//from cmdline
  if (bound < 2) {fprintf(stderr, "ERROR: upper_bound must be at least 2\n"); exit(-1);}

  if (my_rank == 0){
    printf("upper bound: %ld\n", bound);
    printf("processes: %d\n", comm_sz); //display number of processes
  }


  // start time
  timeval start, end;
  MPI_Barrier(MPI_COMM_WORLD); //created barrier before time
  gettimeofday(&start, NULL);

  // execute timed code
  const int maxlen = collatz(bound, comm_sz, my_rank);

  int* global_sum;

  MPI_Reduce(&maxlen, &global_sum, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

  // end time
  gettimeofday(&end, NULL);
  const double runtime = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0;
  if(my_rank == 0){
    printf("compute time: %.5f s\n", runtime);
    // print result
    printf("longest sequence: %d elements\n", global_sum);
  }


  MPI_Finalize(); //finalize MPI


  return 0;
}

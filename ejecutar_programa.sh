#!/bin/bash

NP=${1:-4}

module load mpi/mpich-x86_64
make clean
make
mpirun -np $NP --hostfile mis_hosts.txt ./main

#!/bin/bash

NP=${1:-4}

python3 encontrar_hosts.py
module load mpi/mpich-x86_64
make clean
make
mpirun -np $NP --hostfile mis_hosts.txt ./bin/main

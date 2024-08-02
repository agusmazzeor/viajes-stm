#!/bin/bash

module load mpi/mpich-x86_64
make clean
make

# Valores de NP para iterar
values=(2 4 6 8 12 16)

# Repetir el comando mpirun 5 veces para cada valor de NP
for NP in "${values[@]}"; do
  for i in {1..5}; do
    echo "Corriendo con NP=$NP, iteracion $i"
    mpirun -np $NP --hostfile mis_hosts.txt ./bin/main
  done
done

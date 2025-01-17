#!/bin/bash
#SBATCH --job-name=mitrabajo
#SBATCH --ntasks=2
#SBATCH --mem-per-cpu=16384
#SBATCH --time=10:00:00
#SBATCH --partition=normal
#SBATCH --qos=normal
#SBATCH --mail-type=ALL
#SBATCH --mail-user=natitenreiro@gmail.com

source ~/miniconda3/bin/activate
conda activate cpp_env
module load mpi/mpich-3.2-x86_64

for EJECUCION in 1
do
	echo "Ejecucion $EJECUCION"
	make clean
	make
	mpirun -np 2 ./bin/main
done

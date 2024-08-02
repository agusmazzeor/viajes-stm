#!/bin/bash
#SBATCH --job-name=mitrabajo
#SBATCH --ntasks=8
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
	# echo "1 proceso con 16gb de RAM"
	make clean
	make
	mpirun -np 8 ./bin/main
done

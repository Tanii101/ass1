#!/bin/bash
#SBATCH --job-name=heat_simulation
#SBATCH --output=output.log
#SBATCH --nodes=8
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=4
#SBATCH --time=02:00:00

module load gcc
module load openmpi

mpirun -np 32 ./heat-complete-gcc 256 4096 input_1024.dat frac_1024.dat output_1024_256_4096.dat

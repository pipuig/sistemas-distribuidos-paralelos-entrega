#!/bin/bash
SBATCH -N 1
SBATCH --exclusive
SBATCH -o ./outputPthreads.txt
SBATCH -e ./erroresPthreads.txt
./vectorPthreads $1 $2
./matrizPthreads $1 $2
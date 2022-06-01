#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -o ./output.txt
#SBATCH -e ./errores.txt
./secuencial1dim $1
./secuencial2dim $1
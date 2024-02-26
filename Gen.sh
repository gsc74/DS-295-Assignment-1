#!/bin/bash
#SBATCH --job-name=Generate
#SBATCH --output=Generate.log
#SBATCH --error=Generate.log
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --time=00:30:00


N=1000
P=16


rm A.bin B.bin C.bin

echo "Generate the Matrix"
mpirun -n $P ./Generate $N
echo "Done"

echo "Mutiply the Matrix"
mpirun -n $P ./Mat_Mul $N
echo "Done"



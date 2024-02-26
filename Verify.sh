#!/bin/bash
#SBATCH --job-name=Verification
#SBATCH --output=Verification.log
#SBATCH --error=Verification.log
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=16
#SBATCH --time=00:10:00


N=1000
P=16

# just for test
cp C.bin C_test.bin

echo "Verification"
mpirun -n $P ./Verify $N
echo "Done"



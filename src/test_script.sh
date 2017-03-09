#!/bin/bash

echo "***********Serial Encryption*************"
echo "Char Count: 1080 " 
srun serial/shamir 255 255 < 1080CC.txt
echo "*********Serial Decryption **********"
srun serial/shamir < keys.txt
echo "**********Parallel ENCRYPTION  Strong ************"
echo
echo "Char Count: 1080"
OMP_NUM_THREADS=1 srun par/par_shamir 255 255 < 1080CC.txt
OMP_NUM_THREADS=2 srun par/par_shamir 255 255 < 1080CC.txt
OMP_NUM_THREADS=4 srun par/par_shamir 255 255 < 1080CC.txt
OMP_NUM_THREADS=8 srun par/par_shamir 255 255 < 1080CC.txt
OMP_NUM_THREADS=16 srun par/par_shamir 255 255 < 1080CC.txt
echo "*********Parallel Decryption Strong ***************"
echo "Not running this until we have implemented it"
echo "Testing decryption to make sure it works still"
OMP_NUM_THREADS=1 srun par/par_shamir < keys.txt
#OMP_NUM_THREADS=2 salloc -n 1 mpirun par/par_shamir < keys.txt
#OMP_NUM_THREADS=4 salloc -n 1 mpirun par/par_shamir < keys.txt
#OMP_NUM_THREADS=8 salloc -n 1 mpirun par/par_shamir < keys.txt
#OMP_NUM_THREADS=16 salloc -n 1 mpirun par/par_shamir < keys.txt
echo
echo "**********Encryption Weak Scaling Test ******************"
#OMP_NUM_THREADS=1 	srun par/par_shamir 255 255 < 540CC.txt
#OMP_NUM_THREADS=2 	srun par/par_shamir 255 255 < 1080CC.txt
#OMP_NUM_THREADS=4 	srun par/par_shamir 255 255 < 2160CC.txt
#OMP_NUM_THREADS=8 	srun par/par_shamir 255 255 < 4320CC.txt
#OMP_NUM_THREADS=16 	srun par/par_shamir 255 255 < 8640CC.txt

#!/bin/bash

if [ "$#" -lt 3 ]
then
	echo "Usage: test_script.sh <n shares> <required unlock num> <file to use>"
	exit
fi

echo "***********Serial Encryption*************"
echo "Char Count: "  $3 
srun serial/shamir $1 $2 < $3
echo "Share Keys"
sleep 1
cat keys.txt
echo "*********Serial Decryption **********"
srun serial/shamir < keys.txt
echo "**********Parallel ENCRYPTION  Strong ************"
echo
echo "Char Count: " $3
OMP_NUM_THREADS=1 srun par/par_shamir $1 $2 < $3
OMP_NUM_THREADS=2 srun par/par_shamir $1 $2 < $3
OMP_NUM_THREADS=4 srun par/par_shamir $1 $2 < $3
OMP_NUM_THREADS=8 srun par/par_shamir $1 $2 < $3
OMP_NUM_THREADS=16 srun par/par_shamir $1 $2 < $3
OMP_NUM_THREADS=32 srun par/par_shamir $1 $2 < $3
echo "Share Keys"
sleep 1
cat keys.txt
echo "*********Parallel Decryption Strong ***************"
echo "Not running this until we have implemented it"
echo "Testing decryption to make sure it works still"
echo
OMP_NUM_THREADS=1 srun par/par_shamir < keys.txt
OMP_NUM_THREADS=2 srun par/par_shamir < keys.txt
OMP_NUM_THREADS=4 srun par/par_shamir < keys.txt
OMP_NUM_THREADS=8 srun par/par_shamir < keys.txt
OMP_NUM_THREADS=16 srun par/par_shamir < keys.txt
echo
echo "**********Encryption Weak Scaling Test ******************"
OMP_NUM_THREADS=1 	srun par/par_shamir $1 $2 < 540CC.txt
OMP_NUM_THREADS=2 	srun par/par_shamir $1 $2 < 1080CC.txt
OMP_NUM_THREADS=4 	srun par/par_shamir $1 $2 < 2160CC.txt
OMP_NUM_THREADS=8 	srun par/par_shamir $1 $2 < 4320CC.txt
OMP_NUM_THREADS=16 	srun par/par_shamir $1 $2 < 8640CC.txt

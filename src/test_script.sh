#!/bin/bash

if [ "$#" -lt 3 ]
then
	echo "Usage: test_script.sh <n shares> <required unlock num> <file to use>"
	exit
fi

if [ "$4" = "-d" ]
then
    dump=true
else
    dump=false
fi

echo "***********Serial Key Share Generation *************"
echo "File(char count specified by file name): "  $3 
srun serial/shamir $1 $2 < $3

if $dump ; then
    echo "Share Keys"
    cat keys.txt
fi

echo
echo "*********Serial Secret Join **********"
srun serial/shamir < keys.txt
echo
echo "**********Parallel Key Share Generation- Strong Scale Test************"
echo
echo "Char Count: " $3
OMP_NUM_THREADS=1 	srun par/par_shamir $1 $2 < $3
OMP_NUM_THREADS=2 	srun par/par_shamir $1 $2 < $3
OMP_NUM_THREADS=4 	srun par/par_shamir $1 $2 < $3
OMP_NUM_THREADS=8 	srun par/par_shamir $1 $2 < $3
OMP_NUM_THREADS=16 	srun par/par_shamir $1 $2 < $3
sleep 1

if $dump ; then
    echo "Share Keys Generated"
    cat keys.txt
fi

echo
echo "*********Parallel Secret Join - Strong Scale Test ***************"
echo
sleep 1
OMP_NUM_THREADS=1 	srun par/par_shamir < keys.txt
OMP_NUM_THREADS=2 	srun par/par_shamir < keys.txt
OMP_NUM_THREADS=4 	srun par/par_shamir < keys.txt
OMP_NUM_THREADS=8 	srun par/par_shamir < keys.txt
OMP_NUM_THREADS=16 	srun par/par_shamir < keys.txt
echo "********************************************************************"
echo 
echo "Test that we are actually decrypting"

echo
cat decrypted_file.txt
echo "**********************************************************************"
sleep 1
echo
echo "**********Parallel Key Share Generation -  Weak Scaling Test ******************"
OMP_NUM_THREADS=1 	srun par/par_shamir $1 $2 < test_input_files/540CC.txt
OMP_NUM_THREADS=2 	srun par/par_shamir $1 $2 < test_input_files/1080CC.txt
OMP_NUM_THREADS=4 	srun par/par_shamir $1 $2 < test_input_files/2160CC.txt
OMP_NUM_THREADS=8 	srun par/par_shamir $1 $2 < test_input_files/4320CC.txt
OMP_NUM_THREADS=16 	srun par/par_shamir $1 $2 < test_input_files/8640CC.txt

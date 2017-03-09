How to use our parallel version of Shamir's Secret Sharing

We have provided different input files to encrypt with varying character 
counts, the number of the file for exampe...1080CC.txt, is 1080 characters.

This simulates increasing the problem size for weak scaling test.

USAGE:
To Encrypt:
Either run the provided test_script.sh located in the src directory or

from the source directory:

	OMP_NUM_THREADS=<NTHREADS> srun par_shamir <num of shares> <required unlock num> < <../input file>
	This outputs the shared keys to a file called keys.txt which is required for decryption.
*****NOTE: Limit the num of shares and required unlock num to 255, anything higher causes seg faults.

To Decrypt:
<Parallelism is not implemented for this yet>

par_shamir < keys.txt

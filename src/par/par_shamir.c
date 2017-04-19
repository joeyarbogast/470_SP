/*

	shamir.c -- Shamir's Secret Sharing
	Parallel implementation authors: Joey Arbogast & Isaac Sumner
	Inspired by:

		http://en.wikipedia.org/wiki/Shamir's_Secret_Sharing#Javascript_example


	Compatible with:

		http://www.christophedavid.org/w/c/w.php/Calculators/ShamirSecretSharing

	Notes:

		* The secrets start with 'AABBCC'
		* 'AA' is the hex encoded share # (1 - 255)
		* 'BB' is the threshold # of shares, also in hex
		* 'CC' is fake for compatibility with the web implementation above (= 'AA')
		* Remaining characters are encoded 1 byte = 2 hex characters, plus
			'G0' = 256 (since we use 257 prime modulus)

	Limitations:

		* rand() needs to be seeded before use (see below)


	Copyright © 2015 Fletcher T. Penney. Licensed under the MIT License.

	## The MIT License ##

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#ifdef _OPENMP
#include <omp.h>
#endif
#include <mpi.h>
#include "par_shamir.h"
extern int rank;
extern int nprocs;
int num_threads=1;
static int prime = 257;
/*
	http://stackoverflow.com/questions/322938/recommended-way-to-initialize-srand

	http://www.concentric.net/~Ttwang/tech/inthash.htm
	
	Need a less predictable way to seed rand().
*/

unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

void seed_random(void) {
    //int rank;
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    unsigned long seed = mix(omp_get_wtime(), time(NULL), rank);
    srand(seed);
}

/*
	from http://stackoverflow.com/questions/18730071/c-programming-to-calculate-using-modular-exponentiation

	Allows working with larger numbers (e.g. 255 shares, with a threshold of 200)
*/

int modular_exponentiation(int base,int exp,int mod)
{
    if (exp == 0)
        return 1;
	else if (exp%2 == 0) {
        int mysqrt = modular_exponentiation(base, exp/2, mod);
        return (mysqrt*mysqrt)%mod;
    }
    else
        return (base * modular_exponentiation(base, exp-1, mod))%mod;
}



/*
	split_number() -- Split a number into shares
	n = the number of shares
	t = threshold shares to recreate the number
*/
int * split_number(int number, int n, int t) {
	int *shares;
	int coef[t];
	//int local_coef[t/nprocs];
	int x,i;
	shares = malloc(sizeof(int)*n);
	coef[0] = number;
#	pragma omp parallel shared(nprocs,prime,t,coef,shares) private(number,x,i) 
{
    	num_threads = omp_get_num_threads();
#	pragma omp for schedule(static, (t - 1))
	for (i = 1; i < t; ++i)
	{
	/* Generate random coefficients */
		coef[i] = rand() % (prime - 1);
	}
//    printf("Here I am\n");
//    for (i = 0; i < ((t / nprocs) - 1); ++i)
//    {
//        local_coef[i] = rand() % (prime - 1);
//    }
//    printf("Here I am before gathering\n");
//    MPI_Gather(local_coef, (t / nprocs), MPI_INT, 
 //              coef, (t / nprocs), MPI_INT, 0, MPI_COMM_WORLD);
  //  printf("Here I am again\n");
//#	pragma omp master
//	MPI_Scatter(coef,t/nprocs,MPI_INT,local_coef,t/nprocs,MPI_INT,0,MPI_COMM_WORLD);
#	pragma omp for schedule(static,2)
	for (x = 0; x < n; ++x)
	{
		//int y = local_coef[0];
		int y = coef[0];
		/* Calculate the shares */
		for (i = 1; i < t; ++i)
		{
			int temp = modular_exponentiation(x+1, i, prime);
			y = (y + (coef[i] * temp % prime)) % prime;
		}
		
		/* Sometimes we're getting negative numbers, and need to fix that */
		y = (y + prime) % prime;
		shares[x] = y;
	}
}
//	MPI_Gather(local_shares,n,MPI_INT,shares,n,MPI_INT,0,MPI_COMM_WORLD);
	return shares;  

}
#ifdef TEST
void Test_split_number(CuTest* tc) {

	seed_random();

	int * test = split_number(1234, 50, 20);

	//printf("Split\n1: %d\n2: %d\n3: %d\n4: %d\n5: %d\n6: %d\n", *test, *(test+1), *(test+2),
	//	*(test+3),*(test+4),*(test+5));

	free(test);

	CuAssertIntEquals(tc, 0, 0);
}
#endif

/*
	Math stuff
*/

int * gcdD(int a, int b) {
	int * xyz = malloc(sizeof(int) * 3);

	if (b == 0) {
		xyz[0] = a;
		xyz[1] = 1;
		xyz[2] = 0;
	} else {
		int n = floor(a/b);
		int c = a % b;
		int *r = gcdD(b,c);

		xyz[0] = r[0];
		xyz[1] = r[2];
		xyz[2] = r[1]-r[2]*n;

		free(r);
	}

	return xyz;
}


/*
	More math stuff
*/

int modInverse(int k) {
	k = k % prime;

	int r;
	int * xyz;

	if (k < 0) {
		xyz = gcdD(prime,-k);
		r = -xyz[2];
	} else {
		xyz = gcdD(prime, k);
		r = xyz[2];
	}

	free(xyz);

	return (prime + r) % prime;
}


/*
	join_shares() -- join some shares to retrieve the secret
	xy_pairs is array of int pairs, first is x, second is y
	n is number of pairs submitted
*/

int join_shares(int *xy_pairs, int n) {
	int secret = 0;
	long numerator;
	long denominator;
	long startposition;
	long nextposition;
	long value;
	int i;
	int j;
	//int *local_xy_pairs = malloc(sizeof(int) * n * 2);
	
	


#	pragma omp parallel default(none) shared(num_threads,secret,n,prime,xy_pairs) \
		private(numerator,denominator,value,startposition,nextposition,i,j)
{
	num_threads=omp_get_num_threads();
#	pragma omp for
	for (i = 0; i < n; ++i)
	{
		numerator = 1;
		denominator = 1;
		for (j = 0; j < n; ++j)
		{
			if(i != j) {
				startposition = xy_pairs[i*2];
				nextposition = xy_pairs[j*2];
				numerator = (numerator * -nextposition) % prime;
				denominator = (denominator * (startposition - nextposition)) % prime;
				//fprintf(stderr, "Num: %lli\nDen: %lli\n", numerator, denominator);
			}
		}
		
		value = xy_pairs[i *2 + 1];
#	pragma omp critical		
		secret = (secret + (value * numerator * modInverse(denominator))) % prime;
	}
}

	/* Sometimes we're getting negative numbers, and need to fix that */
	secret = (secret + prime) % prime;

	return secret;
}

#ifdef TEST
void Test_join_shares(CuTest* tc) {
	int n = 200;
	int t = 100;

	int shares[n*2];

	int count = 255;	/* How many times should we test it? */
	int j;

	for (j = 0; j < count; ++j)
	{
		int * test = split_number(j, n, t);
		int i;

		for (i = 0; i < n; ++i)
		{
			shares[i*2] = i + 1;
			shares[i*2 + 1] = test[i];
		}

		/* Send all n shares */
		int result = join_shares(shares, n);

		free(test);

		CuAssertIntEquals(tc, j, result);
	}
}
#endif

/*
	split_string() -- Divide a string into shares
	return an array of pointers to strings;
*/

char ** split_string(char * secret, int n, int t) {
    
   // int rank;
    //MPI_Comm_rank(MPI_COMM_WORLD, &rank);    
    char **shares = malloc(sizeof(char *) * n);
	int len = strlen(secret);
	//MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    //char **loc_shares = malloc(sizeof(char *) * (n / nprocs));
    //int loc_len = len / nprocs;
    //char *loc_secret = malloc(sizeof(char) * loc_len);

    //int i;
	//for (i = 0; i < n; ++i)
	//{
	int i;
//This does work without breaking anything, but the speed up is not drastic(only minor)
//#	pragma omp parallel for default(none) shared(n,t,len,secret,shares) private(i)
	for (i = 0; i < n; ++i)
	{
		/* need two characters to encode each character */
		/* Need 4 character overhead for share # and quorum # */
		/* Additional 2 characters are for compatibility with:
		
			http://www.christophedavid.org/w/c/w.php/Calculators/ShamirSecretSharing
		*/
	//	shares[i] = (char *) malloc(2 * len + 6 + 1);

	//	sprintf(shares[i], "%02X%02XAA", (i+1), t);
	//}
    
    //for (i = 0; i < (n / nprocs); ++i)
    //{
    //    loc_shares[i] = (char *)malloc(2 * len + 6 + 1);
    //    sprintf(loc_shares[i], "%02X%02XAA", (i+1), t);
    //}
    
    /* Now, handle the secret */
    //MPI_Scatter(secret, loc_len, MPI_INT, loc_secret, 
    //            loc_len, MPI_INT, 0, MPI_COMM_WORLD);
	
    //for (i = 0; i < loc_len; ++i)
		shares[i] = (char *) malloc(2*len + 6 + 1);

		sprintf(shares[i], "%02X%02XAA",(i+1),t);
	}
	

	/* Now, handle the secret */
    // This doesn't work I couldn't remember if you tried this
//#   pragma omp parallel for default(none) shared(len, shares, secret, n, t) 
	for (i = 0; i < len; ++i)
	{
		// fprintf(stderr, "char %c: %d\n", secret[i], (unsigned char) secret[i]);
		int letter = secret[i]; // - '0';
		if (letter < 0)
			letter = 256 + letter;

		//fprintf(stderr, "char: '%c' int: '%d'\n", secret[i], letter);
		int * chunks = split_number(letter, n, t);
		int j;
#       pragma omp parallel for
		for (j = 0; j < n; ++j)
		{
			if (chunks[j] == 256) {
				sprintf(shares[j] + 6 + i * 2, "G0");	/* Fake code */
			} else {
				sprintf(shares[j] + 6 + i * 2, "%02X", chunks[j]);				
			}
		}

		free(chunks);
	}
    //MPI_Gather(loc_shares, (n / nprocs), MPI_INT, shares, 
    //           (n / nprocs), MPI_INT, 0, MPI_COMM_WORLD);

	// fprintf(stderr, "%s\n", secret);
	return shares;
}


void free_string_shares(char ** shares, int n) {
	int i;

	for (i = 0; i < n; ++i)
	{
		free(shares[i]);
	}

	free(shares);
}


char * join_strings(char ** shares, int n) {
	/* TODO: Check if we have a quorum */

	if (n == 0)
		return NULL;

	int len = (strlen(shares[0]) - 6) / 2;

	char * result = malloc(len + 1);
	char codon[3];
	codon[2] = '\0';	// Must terminate the string!

	int x[n];
	int i;
	int j;
	for (i = 0; i < n; ++i)
	{
		codon[0] = shares[i][0];
		codon[1] = shares[i][1];
		x[i] = strtol(codon, NULL, 16);
	}
	for (i = 0; i < len; ++i)
	{
		int *chunks = malloc(sizeof(int) * n  * 2);
		for (j = 0; j < n; ++j)
		{
			chunks[j*2] = x[j];

			codon[0] = shares[j][6 + i * 2];
			codon[1] = shares[j][6 + i * 2 + 1];

			if (memcmp(codon,"G0",2) == 0) {
				chunks[j*2 + 1] = 256;
			} else {
				chunks[j*2 + 1] = strtol(codon, NULL, 16);
			}
		}

		//unsigned char letter = join_shares(chunks, n);
		char letter = join_shares(chunks, n);

		free(chunks);

		// fprintf(stderr, "char %c: %d\n", letter, (unsigned char) letter);

		sprintf(result + i, "%c",letter);
	}
	return result;
}


/*
        generate_share_strings() -- create a string of the list of the generated shares,
                one per line
*/

char * generate_share_strings(char * secret, int n, int t) {
        char ** result = split_string(secret, n, t);
        
        int len = strlen(secret);
        int key_len = 6 + 2 * len + 1;
        int i;
        char * shares = malloc(key_len * n + 1);

        for (i = 0; i < n; ++i)
        {
            sprintf(shares + i * key_len, "%s\n", result[i]);
        }
        free_string_shares(result, n);

        return shares;
}

/* Trim spaces at end of string */
void trim_trailing_whitespace(char *str) {
	unsigned long l;
	
	if (str == NULL)
		return;
	
	l = strlen(str);
	
	if (l < 1)
		return;
	
	while ( (l > 0) && (( str[l - 1] == ' ' ) ||
		( str[l - 1] == '\n' ) || 
		( str[l - 1] == '\r' ) || 
		( str[l - 1] == '\t' )) ) {
		str[l - 1] = '\0';
		l = strlen(str);
	}
}


/*
	extract_secret_from_share_strings() -- get a raw string, tidy it up
		into individual shares, and then extract secret
*/

char * extract_secret_from_share_strings(const char * string) {
	char ** shares = malloc(sizeof(char *) * 255);

	char * share;
	char * saveptr = NULL;
	int i = 0;
	/* strtok_rr modifies the string we are looking at, so make a temp copy */
	char * temp_string = strdup(string);

	/* Parse the string by line, remove trailing whitespace */
	share = strtok_rr(temp_string, "\n", &saveptr);

	shares[i] = strdup(share);
	trim_trailing_whitespace(shares[i]);

	while ( (share = strtok_rr(NULL, "\n", &saveptr))) {
		i++;

		shares[i] = strdup(share);

		trim_trailing_whitespace(shares[i]);

		if ((shares[i] != NULL) && (strlen(shares[i]) == 0)) {
			/* Ignore blank lines */
			free(shares[i]);
			i--;
		}
	}

	i++;

	char * secret = join_strings(shares, i);


/*
	fprintf(stdout, "count: %d\n", i);

	for (int j = 0; j < i; ++j)
	{
		fprintf(stderr, "'%s'\n", shares[j]);
	}
*/

	free_string_shares(shares, i);

	return secret;
}

## About ##
|            |                           |	
| ---------- | ------------------------- |
| Title:     | Parallel Shamir's Secret Sharing Algorithm    |
| Serial Version Author:    | Fletcher T. Penney       |
| Date:      | 2017-03-01 |
| Parallel Version Author's: | Joey Arbogast, Isaac Sumner	|
| Version: |1.0|

## Introduction ##

This is an implementation of [Shamir's Secret Sharing][shamir], taken from
github user [fletcher](https://github.com/fletcher/c-sss)

We are working on the parallelization of Fletcher's implementation of Shamir's 
Secret Sharing Algorithm as part of a course project in CS 470: Parallel and Distributed Systems
at [James Madison University](https://w3.cs.jmu.edu/lam2mo/cs470_2017_01/). We have successfully
implemented parallel encryption using OpenMP, without breaking the decryption. We are 
seeing linear speed up times with encryption currently. 

Compiled with -fopenmp and also is setup to use mpi incase we find something that can
be distributed.  

Makefiles included for both parallel and serial versions in their respective directories.

## Current Performance Analysis ##

**All test conducted with maximum shares (255) and max unlock required(255)**
	
**Serial version**

|Char Cnt| Time |
|:-------:|:-----:|
|1080| 9.3400s|


**Parallel Strong Scaling Test**

| Threads | Char Cnt | Time    |
|:-------:|:--------:|:-------:|
| 1       | 1080     | 9.3332s |
| 2       | 1080     | 4.7888s |
| 4       | 1080     | 2.8731s |
| 8       | 1080     | 1.5576s |
| 16      | 1080     | 1.0493s |

**Parallel Weak Scaling Test** 

| Threads | Char Cnt | Time |
|:-------:|:------:|:-----:|
| 1       | 540      | 4.6713s|
| 2       | 1080     | 4.7812s|
| 4       | 2160     | 5.6829s|
| 8       | 4320     | 6.3220s|
| 16      | 8640     | 8.3774s|

### Scheduled For Loops Analysis ###

|Threads | Scheduling | Char Cnt | Time |
|:------:|:----------:|:--------:|:----:|
|        |            |          |      |

## Usage:  ##

Compile with Makefiles located in src/par and src/serial 

There is a tester script located in the src directory for testing scaling

or from the *src* directory:

#### Example: ####

	OMP_NUM_THREADS=8 srun par/par_shamir 255 255 < 1080CC.txt


This will encrypt the text file string and generate 255 shares, all 255 of which are required 
to unlock the secret.  The key shares generated are written to `keys.txt`.  This file should
immediately be separated, since all of the keys together can be used to decrypt the secret.

**DO NOT GO OVER 255 FOR EITHER SHARES OR REQUIRED UNLOCK(causes seg faults)**

There are included test files with varying character lengths specified by the number in the 
text file name.  Use these as inputs into the program to test scaling.

To decrypt: *Parallelization not implemented for this part yet*

#### Example: ####

	srun par/par_shamir < keys.txt


This reads the keys from `keys.txt` and uses them to decrypt the secret, 
which is then output on stdout.


## Project Status ##

We have successfully parallelized the the split_number function using OpenMP directives.  Specifically
we parallelized the random coefficient generation and have parallelized the for loop that calculates
the shares.  Other attempts at parallelizing `join_shares`, `split_string`, `join_strings`, 
`generate_share_strings` and `extract_secret` have proven to be ineffective and break encryption or decryption.

### TODOs ###

* Need to implement parallelism with the decryption.
* Experiment with scheduling and do a performance writeup on it
* See if we can implement MPI in place of OpenMP directives or use MPI on a different 
part of the program.
* Make performance charts and include them in the repository.

## The Shares ##
**From Fletcher github page:**

Each share looks like this:

	0102AA05C0DF2BD6

The first two characters indicate the number of the share (in hex). The
next two characters indicate how many shares are required to decrypt the
secret (again in hex).  The next two characters (`AA`), are fake and enable
the shares to also be used on a website:

<http://www.christophedavid.org/w/c/w.php/Calculators/ShamirSecretSharing>

I cannot vouch for the security of this website -- it claims not to send
any of your information over the internet, but that may or may not be true.


## Security Issues ##
**Fletcher:**

I am not a cryptologist.  From what I can gather from the internet, Shamir's
algorithm is secure -- without a sufficient number of shares, you cannot
"crack the code."  I *believe* that I have implemented the algorithm correctly,
based on what I can read and the fact that it generates compatible results
with another implementation.

Because the secret is encrypted one character at a time, you can use the 
length of the shares to determine the length of the secret.  Depending on
your needs, you could pad the secret with spaces to obscure the actual
length.

Naturally, you have to protect the shares as you would the password, especially
when you have all of them together.

I do not do anything special with the code to guard against any attacks on 
your machine, such as trying to read memory that was used by the application.
If someone has (reasonable) suggestions, I am happy to implement them.  But 
my intent was not to create something that will guard against motivated
attackers with significant resources.

To the best of my knowledge, this program is reasonably safe to use.  You can
readily view the source to ensure that your information is not being sent over
the internet or saved to other files.  But ultimately, it's your responsibility
to use the software carefully.

If you discover any problems with the program, please let me know!


[shamir]:	http://en.wikipedia.org/wiki/Shamir%27s_Secret_Sharing


## Source ##

The serial source is available online:

<https://github.com/fletcher/c-sss>


## Documentation ##

Full documentation is available online:

<http://fletcher.github.io/c-sss/index.html>


## License ##

The `c-sss` project is released under the MIT License.

GLibFacade.c and GLibFacade.h are from the MultiMarkdown v4 project:

	https://github.com/fletcher/MultiMarkdown-4/

MMD 4 is released under both the MIT License and GPL.


CuTest is released under the zlib/libpng license. See CuTest.c for the text
of the license.


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

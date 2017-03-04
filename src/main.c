/*

	main.c -- Template main()

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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "GLibFacade.h"

#include "shamir.h"


char * stdin_buffer() {
	/* Read from stdin and return a char *
		`result` will need to be freed elsewhere */

	GString * buffer = g_string_new("");
	char curchar;
	char * result;

	while ((curchar = fgetc(stdin)) != EOF)
		g_string_append_c(buffer, curchar);

	fclose(stdin);

	result = buffer->str;

	g_string_free(buffer, false);

	return result;
}


int main( int argc, char** argv ) {
	
	seed_random();
	clock_t share_time=0, decrypt_time=0;
	if (argc == 4) {
		// Create shares -- "secret"  n  t 

		char * secret = argv[1];

		int n = atoi(argv[2]);

		int t = atoi(argv[3]);
		share_time = clock();
		char * shares = generate_share_strings(secret, n, t);
		share_time = clock() - share_time;
		fprintf(stdout, "%s", shares);
		

		free(shares);
	} else if (argc == 3) {
		// Read secret from stdin -- n t < cat secret.txt
		char * secret = stdin_buffer();

		int n = atoi(argv[1]);

		int t = atoi(argv[2]);
		share_time = clock();
		char * shares = generate_share_strings(secret, n, t);
		share_time = clock() - share_time;
		fprintf(stdout, "%s\n", shares);

		free(shares);
		free(secret);
	} else {
		// Read shares from stdin -- < shares.txt
		char * shares = stdin_buffer();
		decrypt_time = clock();
		char * secret = extract_secret_from_share_strings(shares);
		decrypt_time = clock() - decrypt_time;
		fprintf(stdout, "%s\n", secret);

		free(secret);

		free(shares);
	}
	fprintf(stdout,"Share time: %.4f   Decrypt time: %.4f\n",
		(float)share_time/(float)CLOCKS_PER_SEC,
		(float)decrypt_time/(float)CLOCKS_PER_SEC);	

}

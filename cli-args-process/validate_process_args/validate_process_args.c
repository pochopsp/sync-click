#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "validate_process_args.h"

void option_0(opts *opts, char* _){
	(void)opts;
	(void)_;
	printf("Hello from option_0!\n");
}

void option_a(opts *opts, char* _){
	(void)opts;
	(void)_;
	printf("Hello from option_a!\n");
}

void option_b(opts *opts, char* _){
	(void)opts;
	(void)_;
	printf("Hello from option_b!\n");
}

void option_c(opts *opts, char* option_argument){
	(void)opts;
	printf("Hello from option_c with option_argument: %s!\n", (char*)option_argument);
}

void set_threads(opts *opts, char* option_argument){
	opts->threads = atoi((char*)option_argument);
	printf("Number of threads set to %d!\n", opts->threads);
}



// Map with the ascii-character[!]
static const lambda anon[] = {
	['0'] = option_0,
	['a'] = option_a,
	['b'] = option_b,
	['c'] = option_c,
	['t'] = set_threads,
};

// // Map every option to its usage[!]
static bool anon_taken[] = {
	['0'] = false,
	['a'] = false,
	['b'] = false,
	['c'] = false,
	['t'] = false,
};

// Map every anon function that requires parameters[!]
static const bool anon_requires_option_argument[256] = {
	['c'] = true,
	['t'] = true,
};

parameters process_arguments(const int argc, char * const * const argv){
	// Start from 2nd option
	int ix=1;

	unsigned char last_key = 0;
	bool wait4option_argument = false;

	parameters params = (parameters){ 
		// At least has to be size argc
		.flags	= calloc(argc, sizeof(flag)),
		.cflags = 0,

		.p_args = calloc(argc, sizeof(char*)),
		.cp_args = 0
	};

	while(ix < argc){
		// TODO fare in modo che si possa dare l'option argument solo se l'ultima option specificata ne riceve uno in input

		char* token = argv[ix];

		// If the argv[ix] starts with -, it has to be a option
		if(token[0] == '-'){
			for(size_t jx=1; jx<strlen(token); jx++){
				const unsigned char key = token[jx];
				if(!anon[key]){
					fprintf(stderr, "Option %c does not exist!\n", key);
					exit(UNKNOWN_OPTION);
				}

				if(anon_taken[key] && !anon_requires_option_argument[key]){
					fprintf(stderr, "Option %c can be specified only once!\n", key);
					exit(ILLEGAL_OPTION);
				}

				anon_taken[key] = true;

				if(anon_requires_option_argument[key]){
					if(wait4option_argument){
						// TODO controllare questo caso
						fprintf(stderr, "Illegal parameters were given to the program!\n");
						exit(ILLEGAL_OPTION);
					}

					last_key = key;
					wait4option_argument = true;
				}
				else {
					params.flags[params.cflags++] = (flag){.lambda = anon[key], .value = NULL };
				}
			}
		} else {
			if(wait4option_argument){
				params.flags[params.cflags++] = (flag){.lambda = anon[last_key], .value = token };
				wait4option_argument = false;
			} else {
				params.p_args[params.cp_args++] = token;
			}
		}
		ix++;
	}

	return params;
}
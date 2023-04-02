#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "process_args.h"

#define ASCII_COUNT 128

processed_argv process_arguments(const int argc, char const * const * const argv, const int optc, option const * const optv){
	
	// option to lambda
	lambda option_lambdas[ASCII_COUNT] = { NULL };
	// option to usage
	bool option_taken[ASCII_COUNT] = { false };
	// option to argument acceptance
	bool accepts_argument[ASCII_COUNT] = { false };
	// option to argument obligatoriness
	bool mandatory_argument[ASCII_COUNT] = { false };

	for(int i = 0; i < optc; ++i){
		option_lambdas[optv[i].option_key] = optv[i].lambda;
		accepts_argument[optv[i].option_key] = optv[i].accepts_arg;
		mandatory_argument[optv[i].option_key] = optv[i].mandatory_arg;
	}
	
	processed_argv processed_args = (processed_argv){ 
		// at least has to be optc
		.options = calloc(optc, sizeof(option)),
		.option_count = 0,

		// at least has to be argc-1
		.params = calloc(argc-1, sizeof(char*)),
		.param_count = 0
	};

	unsigned char latest_opt = 0;
	bool wait_for_argument = false;

	// start from 1 to skip executable name
	int argv_index = 1;

	while(argv_index < argc){
		char* token = argv[argv_index++];

		// if it doesn't start with dash, it has to be either an option argument or a parameter
		if(token[0] != '-'){
			if(wait_for_argument){
				processed_args.options[processed_args.option_count++] = (option){ .lambda = option_lambdas[latest_opt], .option_arg = token };
				wait_for_argument = false;
			} else {
				processed_args.params[processed_args.param_count++] = token;
			}
			continue;
		}

		// here token starts with dash, so it has to be either a single option or many options within the same dash
		for(size_t token_index = 1; token_index < strlen(token); ++token_index){

			if(wait_for_argument){
				if(mandatory_argument[latest_opt]){
					fprintf(stderr, "error: Illegal arguments were given to the program!\n");
					exit(ILLEGAL_OPTION);
				}
				processed_args.options[processed_args.option_count++] = (option){ .lambda = option_lambdas[latest_opt], .option_arg = NULL };
				wait_for_argument = false;
			}

			const unsigned char option_key = token[token_index];				
			if(!option_lambdas[option_key]){
				fprintf(stderr, "error: Option %c does not exist!\n", option_key);
				exit(UNKNOWN_OPTION);
			}

			// option with no arguments specified multiple times does not make sense
			if(option_taken[option_key] && !accepts_argument[option_key]){
				fprintf(stderr, "error: Option %c specified more than once!\n", option_key);
				exit(ILLEGAL_OPTION);
			}

			option_taken[option_key] = true;

			if(accepts_argument[option_key]){
				latest_opt = option_key;
				wait_for_argument = true;
			} else {
				processed_args.options[processed_args.option_count++] = (option){ .lambda = option_lambdas[option_key], .option_arg = NULL };
			}
		}
	}

	if(wait_for_argument){
		if(mandatory_argument[latest_opt]){
			fprintf(stderr, "error: Missing mandatory param for option %c!\n", latest_opt);
			exit(MISSING_ARGMNT);
		} else {
			processed_args.options[processed_args.option_count++] = (option){ .lambda = option_lambdas[latest_opt], .option_arg = NULL };
		}
	}

	return processed_args;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "process_args.h"

#define ASCII_COUNT 128

processed_argv process_arguments(const int argc, char * const * const argv, int optc, option * const optv){
	
	// Map every option to its lambda[!]
	lambda option_functions[ASCII_COUNT] = { NULL };
	// // Map every option to its usage[!]
	bool is_option_taken[ASCII_COUNT] = { false };
	// Map every option that accepts argument[!]
	bool accepts_argument[ASCII_COUNT] = { false };
	// Map every option that requires argument[!]
	bool mandatory_argument[ASCII_COUNT] = { false };

	option *ptr = optv;
	option *endPtr = optv + optc;
	while(ptr < endPtr){
		option_functions[ptr->option_key] = ptr->lambda;
		accepts_argument[ptr->option_key] = ptr->accepts_arg;
		mandatory_argument[ptr->option_key] = ptr->mandatory_arg;
		ptr++;
	}
	
	// start from 1 to skip executable name
	int argv_index = 1;

	processed_argv processed_arguments = (processed_argv){ 
		
		// at least has to be optc
		.options = calloc(optc, sizeof(option)),
		.option_count = 0,

		// at least has to be argc
		.params = calloc(argc, sizeof(char*)),
		.param_count = 0
	};

	unsigned char latest_processed_option = 0;
	bool wait_for_argument = false;

	while(argv_index < argc){
		char* token = argv[argv_index++];

		if(token[0] != '-'){
			// if it doesn't start with dash, it has to be either a option argument or a parameter
			if(wait_for_argument){
				processed_arguments.options[processed_arguments.option_count++] = (option){.lambda = option_functions[latest_processed_option], .option_arg = token };
				wait_for_argument = false;
			} else {
				processed_arguments.params[processed_arguments.param_count++] = token;
			}
			continue;
		}

		// here token starts with dash, so it has to be either an option or concatenated options within the same dash
		for(size_t token_index = 1; token_index < strlen(token); ++token_index){

			if(wait_for_argument){
				if(mandatory_argument[latest_processed_option]){
					fprintf(stderr, "error: Illegal parameters were given to the program!\n");
					exit(ILLEGAL_OPTION);
				}
				processed_arguments.options[processed_arguments.option_count++] = (option){.lambda = option_functions[latest_processed_option], .option_arg = NULL };
				wait_for_argument = false;
			}

			const unsigned char option_key = token[token_index];				
			if(!option_functions[option_key]){
				fprintf(stderr, "error: Option %c does not exist!\n", option_key);
				exit(UNKNOWN_OPTION);
			}

			// option with no arguments specified multiple times does not make sense
			if(is_option_taken[option_key] && !accepts_argument[option_key]){
				fprintf(stderr, "error: Option %c specified more than once!\n", option_key);
				exit(ILLEGAL_OPTION);
			}

			is_option_taken[option_key] = true;

			if(accepts_argument[option_key]){
				latest_processed_option = option_key;
				wait_for_argument = true;
			} else {
				processed_arguments.options[processed_arguments.option_count++] = (option){.lambda = option_functions[option_key], .option_arg = NULL };
			}
		}
	}

	if(wait_for_argument){
		if(mandatory_argument[latest_processed_option]){
			fprintf(stderr, "error: Missing mandatory param for option %c!\n", latest_processed_option);
			exit(MISSING_ARGMNT);
		} else {
			processed_arguments.options[processed_arguments.option_count++] = (option){.lambda = option_functions[latest_processed_option], .option_arg = NULL};
		}
	}

	return processed_arguments;
}
#include <stdio.h>
#include <stdlib.h>

#include "src/process_args.h"

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

void option_t(opts *opts, char* option_argument){
	(void)opts;
	printf("Hello from option_t with option_argument: %s!\n", (char*)option_argument);
}

int main(int argc, char** argv){

	option *options = calloc(4, sizeof(option));

	options[0] = (option){ .option_key = 'a', .accepts_arg = false, .mandatory_arg = false, .lambda = option_a };
	options[1] = (option){ .option_key = 'b', .accepts_arg = false, .mandatory_arg = false, .lambda = option_b };
	options[2] = (option){ .option_key = 'c', .accepts_arg = true, .mandatory_arg = false, .lambda = option_c };
	options[3] = (option){ .option_key = 't', .accepts_arg = true, .mandatory_arg = true, .lambda = option_t };

	processed_argv processed_args = process_arguments(argc, argv, 4, options);
	opts opts = { .threads = 1 };
	
	for(int i=0; i<processed_args.option_count; i++){
		option fnow = processed_args.options[i];
		(void)fnow.lambda(&opts, fnow.option_arg);
	}

	printf("Other parameters: \n");
	for(int i=0; i<processed_args.param_count; i++){
		printf("%s\n", processed_args.params[i]);
	}

	free(processed_args.options);
	free(processed_args.params);
	return 0;
}

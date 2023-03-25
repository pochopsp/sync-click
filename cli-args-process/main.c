#include <stdio.h>
#include <stdlib.h>

#include "src/process_args.h"

void option_a(void *param, char* _){
	(void)param;
	(void)_;
	printf("Hello from option_a!\n");
}

void option_b(void *param, char* _){
	(void)param;
	(void)_;
	printf("Hello from option_b!\n");
}

void option_c(void *param, char* option_argument){
	(void)param;
	printf("Hello from option_c with option_argument: %s!\n", option_argument);
}

void option_t(void *param, char* option_argument){
	(void)param;
	printf("Hello from option_t with option_argument: %s!\n", option_argument);
}

int main(int argc, char** argv){

	// TODO creare degli unit test con https://github.com/ThrowTheSwitch/Unity
	// per testare lo status code riferirsi a https://www.appsloveworld.com/c/100/151/how-to-catch-a-call-to-exit-for-unit-testing
	option *options = calloc(4, sizeof(option));

	options[0] = (option){ .option_key = 'a', .accepts_arg = false, .mandatory_arg = false, .lambda = option_a };
	options[1] = (option){ .option_key = 'b', .accepts_arg = false, .mandatory_arg = false, .lambda = option_b };
	options[2] = (option){ .option_key = 'c', .accepts_arg = true, .mandatory_arg = false, .lambda = option_c };
	options[3] = (option){ .option_key = 't', .accepts_arg = true, .mandatory_arg = true, .lambda = option_t };

	processed_argv processed_args = process_arguments(argc, argv, 4, options);
	
	for(unsigned char i = 0; i < processed_args.option_count; i++){
		option curr_opt = processed_args.options[i];
		curr_opt.lambda(NULL, curr_opt.option_arg);
	}

	printf("Other parameters: \n");
	for(unsigned char i = 0; i < processed_args.param_count; i++){
		printf("%s\n", processed_args.params[i]);
	}

	free(processed_args.options);
	free(processed_args.params);
	return 0;
}

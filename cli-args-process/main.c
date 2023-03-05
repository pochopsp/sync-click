#include <stdio.h>
#include <stdlib.h>

#include "validate_process_args/validate_process_args.h"

int main(int argc, char** argv){
	parameters params = process_arguments(argc, argv);
	opts opts = { .threads = 1 };
	
	for(int i=0; i<params.cflags; i++){
		flag fnow = params.flags[i];
		(void)fnow.lambda(&opts, fnow.value);
	}

	printf("Other parameters: \n");
	for(int i=0; i<params.cp_args; i++){
		printf("%s\n", params.p_args[i]);
	}

	free(params.flags);
	free(params.p_args);
	return 0;
}

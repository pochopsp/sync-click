#include <stdio.h>
#include <stdlib.h>

#include "src/process_args.h"
// TODO citare nella sezione THANKS o DEPENDENCIES del README questa lib e metti la licenza MIT (perché Unity la usa)
//https://github.com/ThrowTheSwitch/Unity
#include "src/unity.h"

void setUp(void) {
    // not used, but impl needed by unity
}

void tearDown(void) {
    // not used, but impl needed by unity
}

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

void myTest(){

	int argc = 7;
	char const * const * argv = (char const * const []){"", "-a", "-b", "-c", "-t", "pecorella", "123"};

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
	
	// TODO rimuovere stampe dalle lambda in modo da popolare parametri e verificarli con assert
	TEST_ASSERT_EQUAL_STRING("123", processed_args.params[0]);

	// TODO verificare memory leak con valgrind
	free(processed_args.options);
	free(processed_args.params);
}

int main(void) {
    // TODO aggiungere vari casi di test
	UNITY_BEGIN();

    RUN_TEST(myTest);
    
	return UNITY_END();
}

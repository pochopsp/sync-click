#include <stdbool.h>

#define UNKNOWN_OPTION 40
#define ILLEGAL_OPTION 41
#define MISSING_ARGMNT 42

typedef struct _opts {
	int threads;
	// ...
} opts;

typedef void (*lambda)(opts*, char*);

typedef struct _option {
	lambda lambda;
	char option_key;
	char* option_arg;
	bool accepts_arg;
	bool mandatory_arg;
} option;

typedef struct _processed_argv {
	// options
	option *options;
	unsigned char option_count;

	// parameters 
	char ** params;
	unsigned char param_count;
} processed_argv;

processed_argv process_arguments(const int argc, char * const * const argv, int optc, option * const optv);
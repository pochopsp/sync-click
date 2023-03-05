#define UNKNOWN_OPTION 40
#define ILLEGAL_OPTION 41

typedef struct _opts {
	int threads;
	// ...
} opts;

typedef void (*lambda)(opts*, char*);

typedef struct _flag{
	lambda lambda;
	char* value;
} flag;

typedef struct _parameters {
	// Flags
	flag *flags;
	unsigned char cflags;

	// Parameters 
	char ** p_args;
	unsigned char cp_args;
} parameters;

parameters process_arguments(const int argc, char * const * const argv);
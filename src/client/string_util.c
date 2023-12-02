#include <stdlib.h>
#include <unistd.h>

// TODO unit testing
char** splitString(char* string, const char delim){

	size_t delimCount = 0;
    char *tmp = string;

    /* Count how many tokens will be extracted. */
    while (*tmp){
		
        if (delim == *tmp)
			++delimCount;
        ++tmp;
    }
	size_t splitCount = delimCount + 1;

    /* Add +1 space for terminating null string so caller
       knows where the list of returned strings ends. */
    ++splitCount;

	char **result = malloc(sizeof(char*) * splitCount);

    if (result){

		char delimString[2]; delimString[0] = delim; delimString[1] = '\0';

        char *token = strtok(string, delimString);

		// Add each extracted token to the list of returned strings 
		int i;
		for(i = 0; i < splitCount-1; ++i){
            *(result + i) = strdup(token);
            token = strtok(NULL, delimString);
        }
		// Add terminating null string to the list of returned strings
        *(result + i) = NULL;
    }

    return result;
}
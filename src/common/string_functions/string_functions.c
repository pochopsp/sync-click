#include <stdbool.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>

long nonnegative_strtol_with_limit(const char *string, int limit){
    char *end;
    errno = 0;
    long val = strtol(string, &end, 10);
    if (errno || end == string || *end != '\0' || val < 0 || val >= limit) {
        return -1;
    }
    return val;
}

bool string_to_uint16(const char *string, uint16_t *res) {
    long val = nonnegative_strtol_with_limit(string, 0x10000);
    if(val == -1) return false;
    *res = (uint16_t)val;
    return true;
}

bool string_to_uint8(const char *string, uint8_t *res) {
    long val = nonnegative_strtol_with_limit(string, 0x100);
    if(val == -1) return false;
    *res = (uint8_t)val;
    return true;
}
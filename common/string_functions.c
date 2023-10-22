#include <stdbool.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>

bool string_to_uint16(const char *string, uint16_t *res) {
    char *end;
    errno = 0;
    long val = strtol(string, &end, 10);
    if (errno || end == string || *end != '\0' || val < 0 || val >= 0x10000) {
        return false;
    }
    *res = (uint16_t)val;
    return true;
}

bool string_to_uint8(const char *string, uint8_t *res) {
    char *end;
    errno = 0;
    long val = strtol(string, &end, 10);
    if (errno || end == string || *end != '\0' || val < 0 || val >= 0x10000) {
        return false;
    }
    *res = (uint8_t)val;
    return true;
}
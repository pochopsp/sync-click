#ifndef STRING_FUNCTIONS_H
#define STRING_FUNCTIONS_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Converts the number represented by string and
 *         puts it in the uint16_t pointed by res.
 *
 *  If the conversion was made successfully (string represents a number from 0 to 65535),
 *  true is returned and the number will be stored in the uint16_t pointer given in input.
 *  Otherwise, false is returned and the pointer will remain untouched.
 *
 *  @param res The pointer to the uint16_t to store the number into.
 *  @param string The pointer to the string to read the number from.
 *  @return true if conversion was made successfully, false in case of error.
 */
bool string_to_uint16(const char *string, uint16_t *res);

/** @brief Converts the number represented by string and
 *         puts it in the uint8_t pointed by res.
 *
 *  If the conversion was made successfully (string represents a number from 0 to 255),
 *  true is returned and the number will be stored in the uint8_t pointer given in input.
 *  Otherwise, false is returned and the pointer will remain untouched.
 *
 *  @param res The pointer to the uint8_t to store the number into.
 *  @param string The pointer to the string to read the number from.
 *  @return true if conversion was made successfully, false in case of error.
 */
bool string_to_uint8(const char *string, uint8_t *res);

#endif // STRING_FUNCTIONS_H
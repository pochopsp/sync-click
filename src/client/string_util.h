#ifndef STRING_UTIL_H
#define STRING_UTIL_H

/** @brief Splits the given string in several strings
 *         by matching the given delimiter.
 *
 *  The string must be filled with two or more substrings separated
 *  by the delim character. The string cannot begin or end with the
 *  delim character.
 *
 *  @param string The pointer for the input string.
 *  @param delim The character to use as delimiter for the split.
 *  @return Pointer to the array storing the strings obtained from the split.
 */
char** splitString(char* string, const char delim);

#endif // STRING_UTIL_H
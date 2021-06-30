/**
 * @file      dlist.c
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Implementation of Doubly Linked List module. It includes
 *              1. inserting elements from both end and start of the list
 *              2. deleting elements from both end and start of the list
 *              3. getting size of the list
 *              4. getting first, last, specific position elements in the list
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>


// ============================================================================
// == | Module Functions
// ============================================================================
/**
 * @brief  Remove the whitespace (in front the first non-space character
 *         and after the last non-space character) of a string
 * 
 * @param  src    a string
 * @return        the string after remove the whitespace 
 */
char *remove_spaces(char *src) {

    char *dest;
    int len = strlen(src);
    int i   = 0;

    // Remove the whitespace after the last non-space character
    while (isspace(src[len - 1]) && len > 0) {
        len -= 1;
    }

    // Remove the whitespace in front the first non-space character
    while (isspace(src[i]) && i < len) {
        len -= 1;
        src += 1;
    }

    // Deep copy the string after removing the whitespace
    dest = deep_copy_str(src, len, !IS_COPY_WHOLE);

    // Return the string after removing the whitespace
    return dest;
}


/**
 * @brief  Deep copy of a string of given length
 * 
 * @param  src          a string will be copied
 * @param  src_len      the length of the string will be copied
 * @param  copy_whole   bool value whether copy the whole string or not
 * @return              the deep copy version of the string
 */
char *deep_copy_str(char *src, int src_len, bool copy_whole) {

    char *dest = (char *)malloc((src_len + 1) * sizeof(char));
    if (dest == NULL) {
        fprintf(stderr, "Error: deep_copy_str() malloc returned NULL\n");
        exit(EXIT_FAILURE);
    }

    if (copy_whole) {
        // If copy the whole targeted string
        strcpy(dest, src);
    } else {
        // If copy a specific length of the target string
        strncpy(dest, src, src_len);
        dest[src_len] = NULL_TERMINATED;
    }

    // Return the string after deep copy
    return dest;
}



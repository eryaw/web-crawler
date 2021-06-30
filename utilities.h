/**
 * @file      utilities.h
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Implementation of Utilities module. It includes
 *              1. remove whitespace of a string
 *              2. deep copy of a string
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdbool.h>


// ============================================================================
// == | Constant Definitions
// ============================================================================
#define MAX_FETCH               100
#define SUCCESS                 0
#define NULL_TERMINATED         '\0'

#define HTTP_HEADER             "http://"
#define SINGLE_SLASH            '/'
#define DOUBLE_SLASH            "//"
#define CRLF                    "\r\n"
#define CRLFCRLF                "\r\n\r\n"
#define SPACE_REGEX_EXP_MAY     "[[:blank:]|[:space:]]{0,}"
#define SPACE_REGEX_EXP_MUST    "[[:blank:]|[:space:]]{1,}"

#define IS_COPY_WHOLE           true


// ============================================================================
// == | Module Functions
// ============================================================================
// Remove whitespaces (in front the first, after the last non-space character) 
// of a string
char *remove_spaces(char *src);

// Deep copy of a string of given length
char *deep_copy_str(char *src, int src_len, bool copy_whole);


#endif

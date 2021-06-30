/**
 * @file      urlHandler.h
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Parsing URL methods. It includes
 *              1. parsing URLs
 *              2. complete the relative URL to absolute one and print it
 *              3. compare two url hostnames, filepath
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#ifndef URLHANDLER_H
#define URLHANDLER_H

#include "dlist.h"

#include "urlInfo.h"

// ============================================================================
// == | Module Functions 
// ============================================================================
// Parsing URL and checking if it is valid and will be handled.
void url_will_be_fetched(char *link,
                        UrlInfo *original,
                        Dlist *waitedList,
                        Dlist *visitedList);


// Parsing the first URL (which is the input)
UrlInfo *parse_first_url(char *link);

// Print out the URL in the Absolute formate (http://hostname/pathname)
void print_url(UrlInfo *url);

// Compare two UrlInfo datas if they are the same 
bool compare_two_URL_diff(UrlInfo *original, UrlInfo *nexturl);


#endif

/**
 * @file      fetchHandler.h
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Parsing HTML method. It includes
 *              1. find and parsing the URL inside anchor tags href field        
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#ifndef HTMLHANDLER_H
#define HTMLHANDLER_H

#include "dlist.h"

#include "urlInfo.h"


// ============================================================================
// == | Module Functions
// ============================================================================
// Parse HTML file, finding and parsing URL inside anchor tags
void parse_html(char *input,
                UrlInfo *original,
                Dlist *waitedList,
                Dlist *visitedList);

#endif

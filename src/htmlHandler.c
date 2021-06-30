/**
 * @file      fetchHandler.c
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Implementation of parsing HTML method. It includes
 *              1. find and parsing the URL inside anchor tags href field         
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#include "htmlHandler.h"

#include "dlist.h"
#include "urlHandler.h"
#include "urlInfo.h"
#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <ctype.h>
#include <regex.h>
#include <string.h>


// ============================================================================
// == | Constant Definitions 
// ============================================================================
#define ANCHOR_TAG_START      "<a"
#define HREF                  "href"
#define ANCHOR_FORMAT         ANCHOR_TAG_START SPACE_REGEX_EXP_MUST
#define HREF_FORMAT           \
        HREF SPACE_REGEX_EXP_MAY "=" SPACE_REGEX_EXP_MAY "(\"|')"


// ============================================================================
// == | Module Functions
// ============================================================================
/**
 * @brief  Parse HTML file, finding and parsing URL inside anchor tags
 * 
 * @param  input        a HTML file
 * @param  original     the UrlInfo data that currently be fetched 
 *                      (the HTML file belong to this URL)
 * @param  waitedList   a dlist of UrlInfo data will be fetched 
 * @param  visitedList  a dlist of UrlInfo data already be fetched 
 */
void parse_html(char *file,
                UrlInfo *original,
                Dlist *waitedList,
                Dlist *visitedList) {

    regex_t     aTag_format, href_format;
    regmatch_t  pmatch[2];
    int         regStatus;

    // Compile the anchor tag start regex format 
    // If compile fail, exit the program
    regStatus = regcomp(&aTag_format, ANCHOR_FORMAT, REG_EXTENDED | REG_ICASE);
    if (regStatus != SUCCESS) {
        fprintf(stderr, "ERROR: compile anchor tag format");
        exit(EXIT_FAILURE);
    }

    // Compile the href field regex format 
    // If compile fail, exit the program
    regStatus = regcomp(&href_format, HREF_FORMAT, REG_EXTENDED | REG_ICASE);
    if (regStatus != SUCCESS) {
        fprintf(stderr, "ERROR: compile href field regex format");
        exit(EXIT_FAILURE);
    }

    // Parsing the HTML file to find URL inside the anchor tag
    // Match the anchor tag start ("<a ")
    while ((regStatus = regexec(&aTag_format, file, 1, &pmatch[0], REG_NOTBOL))
           == SUCCESS) {

        // If there is a match
        // Find the string after the anchor tag start ("<a "), 
        char *currMatch = file + pmatch[0].rm_eo;

        // Match the href field
        if (regexec(&href_format, currMatch, 1, &pmatch[1], REG_NOTBOL) 
            == SUCCESS){
            // If there is a match, find the string after the href field
            currMatch += pmatch[1].rm_eo;

            // Find the start and close qotation mark 
            // It maybe " or ', and it should be same
            char *start_qotation       = currMatch - 1;
            char *after_close_qotation = strchr(currMatch, *start_qotation);

            // Find the URL (between the qotation marks), 
            // and remove the whiteshpace of it
            int link_len  = after_close_qotation - currMatch;
            char *link_sp = deep_copy_str(currMatch, link_len, !IS_COPY_WHOLE);
            char *link    = remove_spaces(link_sp);

            // Parsing URL 
            url_will_be_fetched(link, original, waitedList, visitedList);

            // Free the memory allocation
            free(link_sp);
            link_sp = NULL;
            free(link);
            link = NULL;

            file = after_close_qotation;

        } else {
          // If the anchor tag does not have href field, 
          // Move to the next anchor file
            file = currMatch;
        }
    }

    // Free the regex format
    regfree(&aTag_format);
    regfree(&href_format);
}


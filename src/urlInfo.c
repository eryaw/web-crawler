/**
 * @file      urlInfo.c
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Implementation of URL related information module. It includes
 *              1. creating a new URL data
 *              2. destory and free a URL data
 *              3. deep copy a url
 *            The urlInfo include hostname, file path name, and
 *            if the webpage url direct to required the authorization
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#include "urlInfo.h"

#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>


// ============================================================================
// == | Module Functions
// ============================================================================
/**
 * @brief  Create a new urlInfo data
 * 
 * @return      return a pointer to the new urlInfo data
 */
UrlInfo *new_UrlInfo() {

    UrlInfo *url = (UrlInfo *)malloc(sizeof *url);
    if (url == NULL) {
        fprintf(stderr, "Error: new_UrlInfo() malloc returned NULL\n");
        exit(EXIT_FAILURE);
    }

    // Initalise value of the responseInfo data
    url->hostname        = NULL;
    url->filepath        = NULL;
    url->isAuthorization = false;

    return url;
}


/**
 * @brief  Destroy and free the memory associated with a UrlInfo data
 * 
 * @param  resp   a UrlInfo data
 */
void free_urlInfo(UrlInfo *url) {

    // Error if the UrlInfo does not initalise
    assert(url != NULL);

    // Free the memory associated with a UrlInfo
    free(url->hostname);
    free(url->filepath);
    url->hostname = NULL;
    url->filepath = NULL;

  // Free the UrlInfo data itself
    free(url);
    url = NULL;
}


/**
 * @brief  Deep copy an url inforamtion to another
 * 
 * @param  oldurl   a UrlInfo data will be copied
 * @return          a UrlInfo data after copied
 */
UrlInfo *deep_copy_url(UrlInfo *oldurl){

    UrlInfo *url = new_UrlInfo();
    if (url == NULL) {
        printf("ERROR: new_UrlInfo() malloc returned NULL.");
        exit(EXIT_FAILURE);
    }

    // Get the hostname and filepath of URL will be copied
    char *old_host = oldurl->hostname;
    char *old_file = oldurl->filepath;

    // Deep copy the old UrlInfo data to new UrlInfo data
    url->hostname = deep_copy_str(old_host, strlen(old_host), IS_COPY_WHOLE);
    url->filepath = deep_copy_str(old_file, strlen(old_file), IS_COPY_WHOLE);
    url->isAuthorization = oldurl->isAuthorization;

    return url;
}


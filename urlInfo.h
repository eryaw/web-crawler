/**
 * @file      urlInfo.h
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     URL related information module. It includes
 *              1. creating a new URL data
 *              2. destory and free a URL data
 *            The UrlInfo include hostname, file path name, and
 *            if the webpage url direct to required the authorization
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#ifndef URLINFO_H
#define URLINFO_H

#include <stdbool.h>


// ============================================================================
// == | Data Type Definitions
// ============================================================================
typedef struct link UrlInfo;
/**
 * @brief The UrlInfo include hostname, file path name, and
 *        if the webpage url direct to required the authorization
 */
struct link {
    char *hostname;
    char *filepath;
    bool isAuthorization;
};


// ============================================================================
// == | Module Functions
// ============================================================================
//  Create a new UrlInfo data
UrlInfo *new_UrlInfo();

// Destroy and free the memory associated with a UrlInfo data
void free_urlInfo(UrlInfo *url);

// Deep copy an url inforamtion to another
UrlInfo *deep_copy_url(UrlInfo *oldurl);


#endif
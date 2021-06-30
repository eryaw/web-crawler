/**
 * @file      urlHandler.c
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Implementation of parsing URL methods. It includes
 *              1. parsing URLs 
 *              2. complete the relative URL to absolute one 
 *              3. check if the URL is valid (has valid hostname)
 *              4. compare two url hostnames, filepath
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#include "urlHandler.h"

#include "dlist.h"
#include "fetchHandler.h"
#include "httpHandler.h"
#include "urlInfo.h"
#include "utilities.h"

#include <assert.h>
#include <ctype.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// ============================================================================
// == | Function Prototypes
// ============================================================================
// Parsing URL
UrlInfo *parse_url(char *link, UrlInfo *original);

// Check if URL contains the ignorned characters (./, ../, %, ?, #)
bool ignore_url(char *link);

// Extract the hostname and filepath from a link string
void split_host_file(int title_len, char *link, UrlInfo *url);

// Extract the hostname of the link
void add_new_hostname(char *link, int len, UrlInfo *url);

// Check if the hostname is valid
bool valid_hostname(char *hostname);

// Check if two hostnames are same 
bool compare_hostname(char *original, char *nexturl);

// Check if two hostnames are same for all but first components
bool compare_host_except_fst(char *original, char *nexturl);

// Check if two filepaths except the last trailing slash are same
bool compare_filepath(char *original, char *nexturl);

// Remove the last trailing slash if there has from the filepath
char *omit_last_trailing(char *path);


// ============================================================================
// == | Module Functions 
// ============================================================================
/**
 * @brief  Parsing URL and checking if it is valid and will be handled. 
 *          1. The URL does not contains path segements (./ , ../), characters 
 *             %, #, ? 
 *          2. The URL has same for all but first component hostname compared to
 *             the URL currented be fetched
 *          3. The URL is HTTP protocol only
 *          4. The hostname of URL is valid
 *         If yes and it is never in the URL already be featched and 
 *         waited to be fetched list, insert it into waiting list .
 * 
 * @param  link         a link string
 * @param  original     a UrlInfo data that currently that currently be fetched 
 * @param  waitedList   a dlist of UrlInfo data will be fetched 
 * @param  visitedList  a dlist of UrlInfo data already be fetched 
 */
void url_will_be_fetched(char *link,
                        UrlInfo *original,
                        Dlist *waitedList,
                        Dlist *visitedList) {

    assert(original != NULL);
    assert(waitedList != NULL);
    assert(visitedList != NULL);

    UrlInfo *nexturl;

    if ((nexturl = parse_url(link, original)) != NULL) {
        // Check if the URL satisfies the handle rules

        if (valid_hostname(nexturl->hostname)) {
            // Check if the URL is valid and 

            if (insert_new_Wait(waitedList, visitedList, nexturl)) {
                // if URL is never be fetched before and is unique, insert it
                // into waited list
                return;
            }
        }

        // Free the memory for URL not valid or satisfies the handle rules or
        // already be fetched or waiting list
        free_urlInfo(nexturl);
        nexturl = NULL;
    }
}

/**
 * @brief  Parsing the first URL (which is the input)
 *         The input is valid if it is Absolute URL (fully specified)
 * 
 * @param  link   a link string
 * @return        a UrlInfo data if URL doesn't contain ignored characters 
 *                and it is fully specified Absolute URL
 *                Otherwise, return NULL.
 */
UrlInfo *parse_first_url(char *link) {
    UrlInfo *url;

    if (!ignore_url(link)) {
        // If the link doesn't contain ignored characters

        if ((strncasecmp(link, HTTP_HEADER, strlen(HTTP_HEADER))) == SUCCESS) {
            // If the link is Absolute URL (fully specified)

            url = new_UrlInfo();

            // Extract the hostname and filepath from the link

            split_host_file(strlen(HTTP_HEADER), link, url);

            return url;
        } else {
            fprintf(stderr, "Only URL in <http://<hostname>/<path>> format "
                            "is accepted\n");
        }
    } else {
        fprintf(stderr, "Ignore URL containing path segments(. and "
                        "..), #, ?, %%\n");
    }

    // If the link contain ignored characters or it is not fully specified 
    // Absolute URL, return NULL
    return NULL;
}


/**
 * @brief  Print out the URL in the Absolute formate (http://hostname/pathname)
 * 
 * @param  url    a UrlInfo data
 */
void print_url(UrlInfo *url) {
    assert(url != NULL);

    fprintf(stdout, "%s%s%s\n", 
            HTTP_HEADER, url->hostname, url->filepath);
}


/**
 * @brief  Compare two UrlInfo datas if they are the same 
 *         (same hostname except first component and filepath)
 * 
 * @param  original     the UrlInfo data currently be fetched
 * @param  nexturl      the UrlInfo data may be fetched
 * @return true         If they have same hostname for all but first component 
 *                      and same filepath
 * @return false        If they have different hostname for all but first
 *                      component and same filepath
 */
bool compare_two_URL_diff(UrlInfo *original, UrlInfo *nexturl) {
    if (compare_hostname(original->hostname, nexturl->hostname)
        && !compare_filepath(original->filepath,
                            nexturl->filepath)) {
        return true;
    }
    return false;
}


// ============================================================================
// == | Auxillary Functions 
// ============================================================================
/**
 * @brief  Parsing URL
 *          1. The URL containing path segements (./ , ../), characters 
 *             %, #, ? will be ignored
 *          2. It assume we only receive URL which is
 *              a. Absolute (fully specified):  http://hostname/pathname
 *              b. Absolute (implied protocol): //hostname/pathname
 *              c. Relative:                    /pathname, filename
 * 
 * @param  link         a link string
 * @param  original     a UrlInfo data that currently that currently be fetched
 * @return              a UrlInfo data if URL doesn't contain ignored characters
 *                      If it contains, return NULL.
 */
UrlInfo *parse_url(char *link, UrlInfo *original) {

    UrlInfo *nexturl;

    if (!ignore_url(link)) {
        // If the link doesn't contain ignored characters

        nexturl = new_UrlInfo();

       
        if ((strncasecmp(link, HTTP_HEADER, strlen(HTTP_HEADER))) == SUCCESS) {
            // If the link is Absolute URL (fully specified)
            
            // Extract the hostname and filepath from the link
            split_host_file(strlen(HTTP_HEADER), link, nexturl);

        } else if (link[0] == SINGLE_SLASH 
                    && link[1] == SINGLE_SLASH) {
            // If the link is Absolute URL (implied protocol)

            // Extract the hostname and filepath from the link
            split_host_file(strlen(DOUBLE_SLASH), link, nexturl);

        } else {
            // If the link is Relative URL

            // the hostname of the link should be same as the URL 
            // current be fetched
            nexturl->hostname = deep_copy_str(original->hostname,
                                    strlen(original->hostname), IS_COPY_WHOLE);

            if (link[0] == SINGLE_SLASH) {
                // If the Relative URL is filepath

                nexturl->filepath = deep_copy_str(link, 
                                                strlen(link), IS_COPY_WHOLE);
            } else {
                // If the Relative URL is file name only

                // Extract the filepath exclude the file name of URL 
                // current be fetched (/filpath/filenam.ext )
                char *ori_filefull
                    = omit_last_trailing(original->filepath);
                char *ori_filename = strrchr(ori_filefull, SINGLE_SLASH);

                int len = ori_filename - ori_filefull + 1;
                char *tmp_file
                    = (char *)malloc((len + strlen(link) + 1) * sizeof(char));
                strncpy(tmp_file, ori_filefull, len);
                tmp_file[len] = NULL_TERMINATED;

                // Add the file name after the extracted file path
                // which gives the full filepath for this link
                strcat(tmp_file, link);
                tmp_file[len + strlen(link)] = NULL_TERMINATED;

                nexturl->filepath = tmp_file;

                // Free the memory
                free(ori_filefull);
                ori_filefull = NULL;
            }
        }
        // If the link doesn't ignored characters, return the UrlInfo data
        return nexturl;
    }
    // If the link contains ignored characters, return NULL
    return NULL;
}


/**
 * @brief  Check if URL contains the ignorned characters (./, ../, %, ?, #)
 * 
 * @param  link     the link string
 * @return true     If the URL cotains the ignorned characters 
 * @return false    If the URL does not cotain the ignored characters
 */
bool ignore_url(char *link) {

    if (strstr(link, "./") != NULL 
        || strstr(link, "../") != NULL
        || strchr(link, '?') != NULL 
        || strchr(link, '#') != NULL
        || strchr(link, '%') != NULL) {
        return true;
    } else {
        return false;
    }
}


/**
 * @brief  Extract the hostname and filepath from a link string
 * 
 * @param  protocol_len     length of protocol and/or slashs before 
 *                          hostname/filepath
 * @param  link             a link string
 * @param  url              a UrlInfo data
 */
void split_host_file(int protocol_len, char *link, UrlInfo *url) {

    char *path;

    link += protocol_len;

    if ((path = strchr(link, SINGLE_SLASH)) != NULL) {
        // If there is filepath in the link

        // Extract the filepath from the link
        url->filepath = deep_copy_str(path, strlen(path), false);

        // Extract the hostname from the link
        int len = path - link;
        add_new_hostname(link, len, url);

    } else {
        // If there is no filepath in the link, the default is "/"

        // Add the default filepath "/" to the filepath
        url->filepath = deep_copy_str("/", strlen("/"), false);

        // Extract the hostname from the link
        int len = strlen(link);
        add_new_hostname(link, len, url);
    }
}

/**
 * @brief  Extract the hostname of the link
 * 
 * @param  link   a link string
 * @param  len    the length of the hostname
 * @param  url    a UrlInfo data
 */
void add_new_hostname(char *link, int len, UrlInfo *url) {

    url->hostname = deep_copy_str(link, len, false);
}


/**
 * @brief  Check if the hostname is valid
 * 
 * @param  hostname     a hostname string
 * @return true         If the hostname is valid
 * @return false        If the hostname is invalid
 */
bool valid_hostname(char *hostname) {

    struct hostent *host_info;
    
    // Check if the hostname is valid
    host_info = gethostbyname(hostname);
    if (host_info == NULL) {

        // If it is invalid, return false
        return false;
    }

    // If it is valid, return true
    return true;
}

/**
 * @brief  Check if two hostnames are same 
 *         (including identical, same for all but first component)
 * 
 * @param  original     the hostname of URL currently be fetched
 * @param  nexturl      the hostname of URL may be fetched
 * @return true         If two hostname are same 
 * @return false        If two hostname are different
 */
bool compare_hostname(char *original, char *nexturl) {
    if (strcmp(original, nexturl) == SUCCESS) {
        // If the hostname are identical, return true
        return true;
    }
    if (compare_host_except_fst(original, nexturl)) {
        // If two hostnames are same for all but first components,return false
        return true;
    }

    // If two hostnames are different, return false
    return false;
}


/**
 * @brief  Check if two hostnames are same for all but first components 
 * 
 * @param  original     the hostname of URL currently be fetched
 * @param  nexturl      the hostname of URL may be fetched
 * @return true         If two hostname are same for all but first components
 * @return false        If two hostname are different
 */
bool compare_host_except_fst(char *original, char *nexturl) {
    char *ori_host, *next_host;
    
    if ((ori_host = strchr(original, '.')) != NULL) {
        // If the original hostname has more than one components

        if ((next_host = strchr(nexturl, '.')) != NULL) {
            // If the next hostname has more than one components

            if (strcasecmp(ori_host, next_host) == SUCCESS) {
                return true;
            }
        } else {
            // If the next hostname has only one component

            if (strcasecmp(ori_host, nexturl) == SUCCESS) {
                return true;
            }
        }
    } else {
        // If the original hostname has only one component

        if ((next_host = strchr(nexturl, '.')) != NULL) {
            // If the next hostname has more than one components

            if (strcasecmp(original, next_host) == SUCCESS) {
                return true;
            }
        } else {
            // If the next hostname has only one component

            if (strcasecmp(original, nexturl) == SUCCESS) {
                return true;
            }
        }
    }

    // If two hostname are different, return false
    return false;
}


/**
 * @brief  Check if two filepaths except the last trailing slash are same
 * 
 * @param  original     the filepath of URL currently be fetched
 * @param  nexturl      the filepath of URL may be fetched
 * @return true         If the filepaths except the last trailing slash are same
 * @return false        If the filepath are different
 */
bool compare_filepath(char *original, char *nexturl) {

    // Remove the last trailing slash if there has from the filepath
    char *ori_path  = omit_last_trailing(original);
    char *next_path = omit_last_trailing(nexturl);

    // Check if the filepath except the last trailing slash are same
    if (strcmp(ori_path, next_path) == SUCCESS) {

        // Free the memory
        free(ori_path);
        ori_path = NULL;
        free(next_path);
        next_path = NULL;

        // If two filepath are same, return false
        return true;
    }
    // Free the memory
    free(ori_path);
    ori_path = NULL;
    free(next_path);
    next_path = NULL;

    // If two filepath are different, return false
    return false;
}


/**
 * @brief  Remove the last trailing slash if there has from the filepath
 * 
 * @param  path   a filepath string
 * @return        the string after removing the last trailing slash
 */
char *omit_last_trailing(char *path) {

    char *tmp;
    int path_len = strlen(path);

    if (path[path_len - 1] == SINGLE_SLASH) {
        // If there is a trailing slash at the last of the filepath

        // Get the filepath except the last trailing slash
        tmp = deep_copy_str(path, path_len - 1, !IS_COPY_WHOLE);
    } else {
        // If there is no trailing slash at the last of the filepath

        // Get the full filepath
        tmp = deep_copy_str(path, path_len, IS_COPY_WHOLE);
    }

    // return the string after removing the last trailing slash
    return tmp;
}

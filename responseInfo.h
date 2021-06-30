/**
 * @file      responseInfo.h
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Response related information module. It includes
 *              1. creating a new responseInfo data
 *              2. destory a responseInfo data
 *            The responseInfo include response header, content, status code
 *            content length (if has), content type (if has and is "text/html"), 
 *            redirect link location (if has)
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#ifndef RESPONSEINFO_H
#define RESPONSEINFO_H


// ============================================================================
// == | Data Type Definitions
// ============================================================================
typedef struct http_response ResponseInfo;
/**
 * @brief  A responseInfo include response header, content, status code
 *            content length (if has), content type (if has and is "text/html"), 
 *            redirect link location (if has)
 */
struct http_response {
    char *header;
    int status_code;
    int content_len;
    char *content;
    char *content_type;
    char *redirect_loc;
};


// ============================================================================
// == | Module Functions
// ============================================================================
// Create a new ResponseInfo data
ResponseInfo *new_ResponseInfo();

// Destroy and free the memory associated with a ResponseInfo data
void free_ResponseInfo(ResponseInfo *resp);

// Get response header
char *get_response_content(ResponseInfo *response);


#endif
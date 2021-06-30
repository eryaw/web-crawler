/**
 * @file      responseInfo.c
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Implementation of Response related information module. It includes
 *              1. creating a new responseInfo data
 *              2. destory a responseInfo data
 *            The responseInfo include response header, content, status code
 *            content length (if has), content type (if has and is "text/html"), 
 *            redirect link location (if has)
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#include "responseInfo.h"

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>


// ============================================================================
// == | Module Functions
// ============================================================================
/**
 * @brief  Create a new ResponseInfo data
 * 
 * @return      return a pointer to the new ResponseInfo data
 */
ResponseInfo *new_ResponseInfo() {

    ResponseInfo *resp = (ResponseInfo *)malloc(sizeof *resp);
    if (resp == NULL) {
        fprintf(stderr, "Error: new_ResponseInfo() malloc returned NULL\n");
        exit(EXIT_FAILURE);
    }

    // Initalise value of the responseInfo data
    resp->header       = NULL;
    resp->content      = NULL;
    resp->content_type = NULL;
    resp->redirect_loc = NULL;
    resp->status_code  = 0;
    resp->content_len  = -1;

    return resp;
}


/**
 * @brief  Destroy and free the memory associated with a ResponseInfo data
 * 
 * @param  resp   a ResponseInfo data
 */
void free_ResponseInfo(ResponseInfo *resp) {

    // Error if the ResponseInfo does not initalise
    assert(resp != NULL);

    // Free the memory associated with a responseInfo
    free(resp->header);
    free(resp->redirect_loc);
    resp->header       = NULL;
    resp->content      = NULL;
    resp->content_type = NULL;
    resp->redirect_loc = NULL;

    // Free the ResponseInfo data itself
    free(resp);
    resp = NULL;
}


// Get response header
char *get_response_content(ResponseInfo *resp) { return resp->content; }

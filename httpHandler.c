/**
 * @file      httpHandler.c
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Implementation of HTTP method. It includes
 *              1. send HTTP request
 *              2. recieve HTTP response, including
 *                  a. get the response header
 *                  b. get the response status code
 *                  c. get the field information in header (e.g. Content length)    
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#include "httpHandler.h"

#include "responseInfo.h"
#include "socketHandler.h"
#include "urlInfo.h"
#include "utilities.h"

#include <assert.h>
#include <ctype.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


// ============================================================================
// == | Constant Definitions 
// ============================================================================
#define MAX_RESPONSE_BYTES    100000
#define REQ_USER_AGENT        "eryaw"
#define REQ_GET               "GET"
#define REQ_AUTH_VAL          "Basic ZXJ5YXc6cGFzc3dvcmQ="
#define REQ_AUTHORIZATION     "Authorization: "
#define CONTENT_LEN_HEADER    "Content-Length"
#define CONTENT_TYPE_HEADER   "Content-Type"
#define CONTENT_LOC_HEADER    "Location"
#define ACCEPT_TYPE           "text/html"
#define CONTENT_LEN_FIELD     \
    CONTENT_LEN_HEADER SPACE_REGEX_EXP_MAY ":" SPACE_REGEX_EXP_MAY
#define CONTENT_TYPE_FIELD    \
    CONTENT_TYPE_HEADER SPACE_REGEX_EXP_MAY ":" SPACE_REGEX_EXP_MAY
#define CONTENT_LOC_FIELD     \
    CONTENT_LOC_HEADER SPACE_REGEX_EXP_MAY ":" SPACE_REGEX_EXP_MAY

// The format of part of the HTTP request 
const char *part_of_http_request
    = "%s %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: close\r\n";


// ============================================================================
// == | Function Prototypes
// ============================================================================
// Construct the HTTP request header
char *construct_req_header(UrlInfo *url);

// Extract the response header from whole response
char *extract_header(char *buffer, ResponseInfo *resp);

// Extract the response status code from the header
void extract_status_code(ResponseInfo *resp);

// Extract the Content Length from the header if it has
bool extract_content_length(ResponseInfo *response);

// Extract the Content Type from the header if it has
bool extract_content_type(ResponseInfo *resp);

// Extract the redirect Location from the header if it has
bool extract_content_loc(ResponseInfo *resp);


// ============================================================================
// == | Module Functions 
// ============================================================================
/**
 * @brief  Send HTTP request with GET method
 * 
 * @param  connfd   socket connection ID
 * @param  url      a UrlInfo data
 */
void send_request(int connfd, UrlInfo *url) {
    // Construct the HTTP request header 
    char *header_request = construct_req_header(url);

    // Send HTTP request
    // If fails, exits
    if (write(connfd, header_request, strlen(header_request)) < 0) {
        perror("ERROR sending HTTP request");
        exit(EXIT_FAILURE);
    }

    // Free the memory allocated for header string 
    free(header_request);
    header_request = NULL;
}


/**
 * @brief  Recieve HTTP response from server 
 *         And get response header, status code and other field information 
 *         according to the status code 
 *         It will only handle response which 
 *            1. status code is one of 200, 301, 401, 404, 410, 414, 503, 504
 *            2. MIME-Type is "text/html"
 *            3. No truncated Pages (Content length equal to actual length)
 * 
 * @param  connfd   socket connection ID
 * @param  resp     a ResponseInfo data
 * 
 * @return true     If status code will be handled 
 *                  and it is 200, 301, 401, 503
 *                  and it satisfies the 3 handle rules listed above
 * @return false    If status code will not be handled 
 *                  or it is 410, 404, 414, 504
 *                  or it does not satisfies the 3 handle rules listed above
 */
bool get_response_from_server(int connfd, ResponseInfo *resp) {

    int nbytes = -1;
    int bufferUsed = 0;

    // Initialise the buffer for response
    char buffer[MAX_RESPONSE_BYTES];
    bzero(buffer, MAX_RESPONSE_BYTES);

    // Recieve the HTTP response 
    // The maximum response bytes it will handle is 100000
    while ((nbytes = read(connfd, &buffer[bufferUsed],
                          MAX_RESPONSE_BYTES - bufferUsed - 1))
           > 0) {

        // Check if it still have enough buffer for response
        bufferUsed += nbytes;
        if (bufferUsed == MAX_RESPONSE_BYTES - 1) {
            break;
        }
    }

    // If the response fails, exits
    if (nbytes == -1) {
        perror("ERROR receiving resp");
        exit(EXIT_FAILURE);
    }

    // Close the socket connection
    close_socket(connfd);


    // Get the response header
    char *content = extract_header(buffer, resp);
    int len = strlen(content);

    // If the response header exists, check the status code and other 
    // field information according to the status code 
    if (resp->header != NULL) {

        // Get the status code 
        extract_status_code(resp);

        if (resp->status_code == 200) {
            /** If the status code is 200 OK
             * Get the Content Length and Content Type. 
             * Check if the MIME-Type is the accpeted type and if it is 
             * truncated pages.
            */
            if (content != NULL 
                && extract_content_type(resp)
                && extract_content_length(resp)) {
                
                if (len == resp->content_len) {

                    // If the content length in header is equal to the actual  
                    // content length, it is not truncated pages. 
                    // Return true as it will be handled
                    resp->content = content;
                    return true;
                }
            }
        } else if (resp->status_code == 301) {
            /** If the status code is 301 Moved Permanently 
             *  Check if there is a redirect Location. 
             *  If yes, return true as it will be handled
            */
            if (extract_content_loc(resp)) {
                return true;
            }
        } else if (resp->status_code == 401 
                || resp->status_code == 503) {
            /** If the status code is 401 Unauthorized Error, 
             *  or 503 Service Unavailable
             *  Return true as it will be handled
            */
            return true;
        }
    }

    // If the response will not be handle, return false
    return false;
}


// ============================================================================
// == | Auxillary Functions 
// ============================================================================
/**
 * @brief  Construct the HTTP request header
 * 
 * @param  url    a UrlInfo data
 * @return        the full HTTP request header string
 */
char *construct_req_header(UrlInfo *url) {

    // Error if the UrlInfo data does not exist
    assert(url != NULL);

    // Get the hostname and file path name of the URL
    char *hostname = url->hostname;
    char *filepath = url->filepath;

    char *header_request;

    // Get the header length with request information (e.g. hostname, filepath)
    int header_len = strlen(part_of_http_request) 
                  + strlen(REQ_GET)
                  + strlen(filepath) 
                  + strlen(hostname)
                  + strlen(REQ_USER_AGENT) 
                  + strlen(CRLF) 
                  + 1;
    header_request = (char *)malloc(header_len * sizeof(char));
    if (header_request == NULL) {
        fprintf(stderr, "Error: construct_req_header() malloc returned NULL\n");
        exit(EXIT_FAILURE);
    }

    // Fill the request information into the HTTP request header
    sprintf(header_request, part_of_http_request, 
            REQ_GET, filepath, hostname, REQ_USER_AGENT);

    // If the Authorization information is required in the HTTP request
    if (url->isAuthorization) {

        // Add the Authorization information length
        header_len += strlen(REQ_AUTHORIZATION) + strlen(REQ_AUTH_VAL);

        header_request = (char *)realloc(header_request, header_len);

        // Construct the Authorization field in request header
        strcat(header_request, REQ_AUTHORIZATION);
        strcat(header_request, REQ_AUTH_VAL);
        strcat(header_request, CRLF);
    }

    // Construct the end of the request header 
    strcat(header_request, CRLF);

    return header_request;
}


/**
 * @brief  Extract the response header from whole response
 * 
 * @param  buffer   the whole response
 * @param  resp     a ResponseInfo data
 * @return          the content of the response. 
 *                  If header is incomplete or there is no content, return NULL
 */
char *extract_header(char *buffer, ResponseInfo *resp) {

    // Get the content of the response 
    char *content = strstr(buffer, CRLFCRLF);

    if (content != NULL) {

        int content_len = strlen(content);

        // Ensure each field line in header end with \r\n
        int header_len = strlen(buffer) - content_len + strlen(CRLF);

        // Get the response header 
        char *header = deep_copy_str(buffer, header_len, !IS_COPY_WHOLE);
        resp->header       = header;

        // Return the content
        content += strlen(CRLFCRLF);
        return content;
    }

    // If header is incomplete or there is no content, return NULL
    return NULL;
}


/**
 * @brief  Extract the response status code from the heade
 * 
 * @param  resp   a ResponseInfo data
 */
void extract_status_code(ResponseInfo *resp) {
    int status_code;
    sscanf(resp->header, "HTTP/1.1 %d \r\n", &status_code);
    resp->status_code = status_code;
}

/**
 * @brief  Extract the Content Length from the header if it has
 * 
 * @param  resp   a ResponseInfo data
 * @return true   the Content-Length field exist and content length is found
 * @return false  the Content-Length field does not exist
 */
bool extract_content_length(ResponseInfo *resp) {

    regex_t cLengthFormat;
    regmatch_t pmatch;
    
    int cLength;
    int regStatus;

    char *header = resp->header;

    // Compile the Content length field name format 
    // If compile fail, exit the program
    regStatus
        = regcomp(&cLengthFormat, CONTENT_LEN_FIELD, REG_EXTENDED | REG_ICASE);
    if (regStatus != SUCCESS) {
        fprintf(stderr, "ERROR: compile the Content length field name format ");
        exit(EXIT_FAILURE);
    }

    // Parsing the header to find the Content-Length field
    if (regexec(&cLengthFormat, header, 1, &pmatch, REG_NOTBOL) == SUCCESS) {

        // find the content length and convert it into Integer
        sscanf(header + pmatch.rm_eo, "%d \r\n", &cLength);
        resp->content_len = cLength;

        // Free the regex format
        regfree(&cLengthFormat);

        // the Content-Length field exist, return false
        return true;
    }

    // Free the regex format
    regfree(&cLengthFormat);

    // the Content-Length field does not exist, return false
    return false;
}

/**
 * @brief  Extract the Content Type from the header if it has
 * 
 * @param  resp   a ResponseInfo data
 * @return true   if the Content-Type field exists and the type is "text/html"
 * @return false  if the Content-Type field not exist or is not accpeted type
 */
bool extract_content_type(ResponseInfo *resp) {

    regex_t cTypeFormat;
    regmatch_t pmatch;

    int regStatus;

    char *header = resp->header;
    char *ctype;
    char *after_ctype;
    
    // Compile the Content type field name format 
    // If compile fail, exit the program
    regStatus
        = regcomp(&cTypeFormat, CONTENT_TYPE_FIELD, REG_EXTENDED | REG_ICASE);
    if (regStatus != SUCCESS) {
        fprintf(stderr, "ERROR: compile the Content type field name format");
        exit(EXIT_FAILURE);
    }

    // Parsing the header to find the Content-Type field
    if (regexec(&cTypeFormat, header, 1, &pmatch, REG_NOTBOL) == SUCCESS) {
        if ((after_ctype = strstr(header + pmatch.rm_eo, CRLF)) != NULL) {

            // Get the content type field line
            int len = after_ctype - (header + pmatch.rm_eo);
            ctype = deep_copy_str(header + pmatch.rm_eo, len, !IS_COPY_WHOLE);

            if (strstr(ctype, ACCEPT_TYPE) != NULL) {
          
                // if content type is "text/html", return true
                resp->content_type = ACCEPT_TYPE;

                // Free the memory and regex format
                free(ctype);
                ctype = NULL;
                regfree(&cTypeFormat);

                // If the Location field exists, return true
                return true;
            }

            // Free the memory
            free(ctype);
            ctype = NULL;
        }
    }

    // Free the regex format
    regfree(&cTypeFormat);

    // if Content-Type field not exist or type is not "text/html", return false
    return false;
}

/**
 * @brief  Extract the redirect URL Location from the header if it has
 * 
 * @param  resp   a ResponseInfo data
 * @return true   if the Location field exists 
 * @return false  if the Location field not exist
 */
bool extract_content_loc(ResponseInfo *resp) {
    regex_t cLocationFormat;
    regmatch_t pmatch;

    int regStatus;

    char *header = resp->header;
    char *cLoc_sp, *cLoc;
    char *aftercLoc;

    // Compile the Location field name format 
    // If compile fail, exit the program
    regStatus = regcomp(&cLocationFormat, CONTENT_LOC_FIELD,
                              REG_EXTENDED | REG_ICASE);
    if (regStatus != SUCCESS) {
        fprintf(stderr, "ERROR: compile the Location field name format");
        exit(EXIT_FAILURE);
    }

    // Parsing the header to find the Location field
    if (regexec(&cLocationFormat, header, 1, &pmatch, REG_NOTBOL) == SUCCESS) {

        if ((aftercLoc = strstr(header + pmatch.rm_eo, CRLF)) != NULL) {

            // Get the Location line and find the redirect URL
            int len = aftercLoc - (header + pmatch.rm_eo);
            cLoc_sp = deep_copy_str(header + pmatch.rm_eo, len, !IS_COPY_WHOLE);

            // Remove whitespaces of the redirect URL
            cLoc    = remove_spaces(cLoc_sp);

            resp->redirect_loc = cLoc;

             // Free the memory and regex format
            free(cLoc_sp);
            cLoc_sp = NULL;
            regfree(&cLocationFormat);

            // If the Location field exists, return true
            return true;
        }
    }

    // Free the regex format
    regfree(&cLocationFormat);

    // If the Location field does not exist, return false
    return false;
}

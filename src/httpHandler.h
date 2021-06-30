/**
 * @file      httpHandler.h
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     HTTP method. It includes
 *              1. send HTTP request
 *              2. recieve HTTP response
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include "responseInfo.h"
#include "urlInfo.h"

#include <stdbool.h>


// ============================================================================
// == | Module Functions
// ============================================================================
// Send HTTP request with GET method
void send_request(int connfd, UrlInfo *url);

// Recieve HTTP response from server and extract header, status code 
// and other field information according to the status code 
bool get_response_from_server(int connfd, ResponseInfo *response);


#endif

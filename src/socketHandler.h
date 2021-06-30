/**
 * @file      socketHandler.h
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Socket connection module. It includes
 *              1. set up and connect socket 
 *              2. close the socket connection
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

// ============================================================================
// == | Module Functions
// ============================================================================
// Set the up socket object and connect
int setup_socket(char *hostname);

// Close the socket connectoin
void close_socket(int connfd);


#endif

/**
 * @file      socketHandler.c
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Implementation of socket connection module. It includes
 *              1. set up and connect socket 
 *              2. close the socket connection
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#include "socketHandler.h"

#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// ============================================================================
// == | Constant Definitions 
// ============================================================================
#define SERVER_PORT 80

// ============================================================================
// == | Module Functions 
// ============================================================================
/**
 * @brief Set the up socket object and connect
 * 
 * @param  hostname     a string of hostname
 * @return              the socket conncection ID
 */
int setup_socket(char *hostname) {
    
    int connfd;

    struct sockaddr_in serv_addr;
    struct hostent *host_info;

    // Create socket
    connfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connfd < 0) {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    // Get IP address from the hostname
    // If the hostname is invalid, exits
    host_info = gethostbyname(hostname);
    if (host_info == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }

    // Set up the socket and host information
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(host_info->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr,
          host_info->h_length);
    serv_addr.sin_port = htons(SERVER_PORT);

    // Connect the socket
    // If the connection fails, exits
    if (connect(connfd, (struct sockaddr *)&serv_addr,
                sizeof(struct sockaddr_in))
        < 0) {
        perror("ERROR connecting");
        exit(EXIT_FAILURE);
    }

    // Return the socket connection ID
    return connfd;
}


/**
 * @brief  Close the socket connectoin
 * 
 * @param  connfd   the socket connection ID
 */
void close_socket(int connfd) {
    // Close the socket connectoin
    // If close connection fails, exits
    if (close(connfd) < 0) {
        perror("ERROR Disconnect Socket");
        exit(EXIT_FAILURE);
    }
}
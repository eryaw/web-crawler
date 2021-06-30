/**
 * @file      main.c
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Web Crawler
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#include "dlist.h"
#include "fetchHandler.h"
#include "httpHandler.h"
#include "htmlHandler.h"
#include "responseInfo.h"
#include "socketHandler.h"
#include "urlInfo.h"
#include "urlHandler.h"
#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>


// ============================================================================
// == | Function Prototypes
// ============================================================================
// Loop crawling the webpages and fetching the URLs
void loop_fetching(UrlInfo *url) ;


// ============================================================================
// == | Main Functions
// ============================================================================
/**
 * @brief  Crawl the webpage to find and fetch URLs
 * 
 * @param  argc   number of inputs
 * @param  argv   an array of inputs
 * @return        if no fail exits, return 0
 */
int main(int argc, char** argv) {

    // If the input is incorrect, exits
    if (argc != 2){
        fprintf(stderr, "Usage: %s <URL> \n", argv[0]);
		exit(EXIT_FAILURE);
    }

    // Parse the first URL which from the input
    UrlInfo *url = parse_first_url(argv[1]);

    
    if (url != NULL){
        // Loop crawling the webpages and fetching the URLs
        loop_fetching(url);
    }

    return 0;
}

// ============================================================================
// == | Auxillary Functions 
// ============================================================================
/**
 * @brief  Loop crawling the webpages and fetching the URLs
 * 
 * @param  url    a UrlInfo data
 */
void loop_fetching(UrlInfo *url) {

    // Initialise the URL already be fetched and will be fetched dlist
    Dlist *waitedList = new_Wait();
    Dlist *visitedList = new_Visited();

    // Insert the first be fetched URL
    insert_new_Wait(waitedList, visitedList, url);
    int waitsize = get_dlist_size(waitedList);
    int visitsize = get_dlist_size(visitedList);
    
    // The maximum number of fetching is 100
    while(waitsize > 0 && visitsize < MAX_FETCH){

        // Get a URL from the URL will be fetched dlist
        url = dlist_remove_start(waitedList);

        // Set up socket and connect it
        int connfd = setup_socket(url->hostname);

        // Fetched the URL by sending HTTP request to server
        send_request(connfd, url);
        insert_new_Visit(visitedList, url);
        

        ResponseInfo *resp = new_ResponseInfo();

        // If it is valid and satisfies the handle rules, we will get
        // response from the server
        if(get_response_from_server(connfd, resp)){

            if (resp->status_code == 200){
                /** If the status code is 200 OK
                 * Parsing the HTML file of the content to find the URLs
                 * And parsing URLs 
                 */
                char *content = resp->content;
                
                parse_html(content, url, waitedList, visitedList);

                // If the URL is valid and unique(never fetched before), 
                // add to the URL will be fetched list
                insert_new_Wait(waitedList, visitedList, url);
                
            } else if (resp->status_code == 503){
                /** If the status code is 503 Service Unavailable
                 * Add the URL to the URL will be fetched list
                 * It will be refetching
                 */
                UrlInfo *newurl = deep_copy_url(url);
                dlist_add_start(waitedList, newurl); 

            } else if (resp->status_code == 301){
                /** If the status code is 301 Moved Permanently 
                 * Find the redirect link from the response and parsing it
                 */
                url_will_be_fetched(resp->redirect_loc, 
                                    url, 
                                    waitedList, 
                                    visitedList);

            } else if(resp->status_code == 401){
                /** If the status code is 401 Unauthorized Error
                 * Refetching it 
                 * and send the HTTP request with Authorization information 
                 */
                url->isAuthorization = true;
                UrlInfo *newurl = deep_copy_url(url);
                dlist_add_start(waitedList, newurl);
            }
        }

        // Get the current number of URL in the URL already be fetched dlist
        // and the URL will be fetched dlist
        waitsize = get_dlist_size(waitedList);
        visitsize = get_dlist_size(visitedList);
        
        // free the memory of responseInfo data 
        free_ResponseInfo(resp);
    }

    // Print out all the fetched URLs
    for (int i = 0; i < visitsize; i++){
        UrlInfo *url = get_dlist_point(visitedList, i);
        print_url(url);
    }

    // free the dlists of the URL already be fetched and will be fetched 
    free_dlist(waitedList);
    free_dlist(visitedList);
}


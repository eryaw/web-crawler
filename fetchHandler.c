/**
 * @file      htmlHandler.c
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Implementation of fetching method. It includes
 *              1. initialising doubly linked list of already be fetched URLs
 *                 and inserting elements into it
 *              2. initialising doubly linked list of URLs will be fetched
 *                 and inserting elements into it
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#include "fetchHandler.h"

#include "dlist.h"
#include "urlHandler.h"
#include "urlInfo.h"
#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>
#include <string.h>


// ============================================================================
// == | Module Functions
// ============================================================================
/**
 * @brief  Create new list of URLs which already be fetched 
 * 
 * @return The address of the list
 */
Dlist *new_Visited() {
    Dlist *visitedList = new_dlist();
    return visitedList;
}


/**
 * @brief  Create new list of URLs which will be fetched
 * 
 * @return The address of the list
 */
Dlist *new_Wait() {
    Dlist *waitedList = new_dlist();
    return waitedList;
}


/**
 * @brief  Insert the UrlInfo data which will be fetched into list
 * 
 * @param  waitedList   a dlist of UrlInfo data will be fetched 
 * @param  vistedList   a dlist of UrlInfo data already be fetched 
 * @param  nexturl      a UrlInfo data
 * 
 * @return true         If the UrlInfo data not exist in the list,
 *                      and be inserted into the list successfully
 * @return false        If the UrlInfo data already in the list or be fetched
 */
bool insert_new_Wait(Dlist *waitedList, 
                     Dlist *vistedList, 
                     UrlInfo *nexturl) {

    UrlInfo *existUrl;

    int waitSize  = get_dlist_size(waitedList);
    int visitSize = get_dlist_size(vistedList);

    // Compare the new UrlInfo data with each data already in the waiting list 
    // to ensure there is no duplicate URL
    // If the data already in the list, then return false 
    for (int i = 0; i < waitSize; i++) {
        existUrl = get_dlist_point(waitedList, i);
        if (!compare_two_URL_diff(existUrl, nexturl)) {
            return false;
        }
    }

    // Compare the new UrlInfo data with each data already be fetched 
    // to ensure the same webpage will only be fetched once
    // If the data already in the list, then return false
    for (int i = 0; i < visitSize; i++) {
        existUrl = get_dlist_point(vistedList, i);
        if (!compare_two_URL_diff(existUrl, nexturl)) {
            return false;
        }
    }

    // If the URL is not be fetched or already in the waiting list, 
    // insert it into the waiting list, and return true
    dlist_add_start(waitedList, nexturl);
    return true;
}


/**
 * @brief  Insert the already be fetched UrlInfo data into the list 
 * 
 * @param  vistedList   a dlist of UrlInfo data already be fetched 
 * @param  nexturl      UrlInfo data already be fetched
 */
void insert_new_Visit(Dlist *vistedList, UrlInfo *nexturl) {

    int visitSize = get_dlist_size(vistedList);

    // Insert the data if the size of list haven't reached the maximum
    if (visitSize < MAX_FETCH) {
        dlist_add_end(vistedList, nexturl);
    }
}
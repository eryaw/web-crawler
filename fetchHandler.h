/**
 * @file      fetchHandler.h
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Fetching method. It includes
 *              1. initialising doubly linked list of already be fetched URLs
 *                 and inserting elements into it
 *              2. initialising doubly linked list of URLs will be fetched
 *                 and inserting elements into it
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#ifndef FETCHHANDLER_H
#define FETCHHANDLER_H

#include "dlist.h"

#include "urlInfo.h"


// ============================================================================
// == | Module Functions
// ============================================================================
// Create new list of URLs which already be fetched 
Dlist *new_Visited();

// Create new list of URLs which will be fetched
Dlist *new_Wait();

// Insert the UrlInfo data which will be fetched into list
bool insert_new_Wait(Dlist *waitedList, 
                     Dlist *visitedList, 
                     UrlInfo *nexturl);

// Insert the already be fetched UrlInfo data into the list 
void insert_new_Visit(Dlist *vistedList, UrlInfo *nexturl);


#endif

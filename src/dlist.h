/**
 * @file      dlist.h
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Doubly Linked List module. It includes
 *              1. inserting elements from both end and start of the list,
 *              2. deleting elements from both end and start of the list,
 *              3. getting size of the list,
 *              4. getting first, last, specific position elements in the list
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#ifndef DLIST_H
#define DLIST_H


#include "urlInfo.h"

#include <stdbool.h>


// ============================================================================
// == | Data Type Definitions
// ============================================================================
typedef struct dlist Dlist;


// ============================================================================
// == | Module Functions
// ============================================================================
// Create a new list and return its pointer
Dlist *new_dlist();

// Destroy a list and free its memory
void free_dlist(Dlist *dlist);

// Add an element to the front of a list
void dlist_add_start(Dlist *dlist, UrlInfo *url);

// Add an element to the back of a list
void dlist_add_end(Dlist *dlist, UrlInfo *url);

// Remove and return the front data element from a list
UrlInfo *dlist_remove_start(Dlist *dlist);

// Remove and return the final data element in a list
UrlInfo *dlist_remove_end(Dlist *dlist);

// Return the number of elements contained in a list
int get_dlist_size(Dlist *dlist);

// Return the data of a given index UrlInfo from a Dlist
UrlInfo *get_dlist_point(Dlist *dlist, int index);


#endif

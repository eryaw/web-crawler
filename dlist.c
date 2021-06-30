/**
 * @file      dlist.c
 * @author    Erya Wen (eryaw@student.unimelb.edu.au)
 * @brief     Implementation of Doubly Linked List module. It includes
 *              1. creating new doubly linked list
 *              2. destory a doubly linked list
 *              3. inserting elements from both end and start of the list
 *              4. deleting elements from both end and start of the list
 *              5. getting size of the list
 *              6. getting first, last, specific position elements in the list
 *
 * @copyright created for COMP30023 Computer System 2020
 *
 */

#include "dlist.h"

#include "urlInfo.h"

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>


// ============================================================================
// == | Data Type Definitions
// ============================================================================
typedef struct node Node;
/**
 * @brief  A dlist node points to the next and previous node in the dlist,
 *         which data is UrlInfo datatype
 */
struct node {
    struct node *prev;
    struct node *next;
    UrlInfo *url;
};


/**
 * @brief  A dlist points to its first and last nodes,
 *         and stores its size (number of nodes)
 */
struct dlist {
    Node *head;
    Node *last;
    int size;
};


// ============================================================================
// == | Function Prototypes
// ============================================================================
// Create a new node and return its address
Node *new_node(UrlInfo *url);


// ============================================================================
// == | Module Functions
// ============================================================================
/**
 * @brief  Create a new empty Dlist 
 * 
 * @return        the pointer of new empty Dlist
 */
Dlist *new_dlist() {

    Dlist *dlist = (Dlist *)malloc(sizeof *dlist);
    if (dlist == NULL) {
        fprintf(stderr, "Error: new_dlist() malloc returned NULL\n");
        exit(EXIT_FAILURE);
    }

    // Initalise value of the dlist
    dlist->head = NULL;
    dlist->last = NULL;
    dlist->size = 0;

    return dlist;
}


/**
 * @brief  Destroy and free the memory associated with a Dlist
 *
 * @param  dlist  a Dlist
 */
void free_dlist(Dlist *dlist) {

    // Error if the list does not initalise
    assert(dlist != NULL);

    // Free UrlInfo data in each nodes.
    int size = get_dlist_size(dlist);
    while (size > 0) {
        UrlInfo *url = dlist_remove_start(dlist);
        free_urlInfo(url);
        size = get_dlist_size(dlist);
    }

    // Free the last node and dlist itself
    free(dlist->last);
    dlist->head = NULL;
    dlist->last = NULL;

    free(dlist);
    dlist = NULL;
}


/**
 * @brief  Add a UrlInfo data to the end of a Dlist 
 *
 * @param  dlist  a Dlist
 * @param  url    a UrlInfo data
 */
void dlist_add_end(Dlist *dlist, UrlInfo *url) {

    // Error if the list or url does not initalise
    assert(dlist != NULL);
    assert(url != NULL);

    // Creat a new dlist node to store this data
    Node *node = new_node(url);

    // Connect the node with the current last node
    node->prev = dlist->last;

    if (dlist->size == 0) {
        // If the dlist was empty, new node is now the first node
        dlist->head = node;
    } else {
        // Otherwise, it goes after the current last node
        dlist->last->next = node;
    }

    // Place this new node at the end of the dlist
    dlist->last = node;

    // Update the dlist size
    dlist->size++;
}

/**
 * @brief  Add a UrlInfo data to the front of a Dlist 
 * 
 * @param  dlist  a Dlist
 * @param  url    a UrlInfo data
 */
void dlist_add_start(Dlist *dlist, UrlInfo *url) {

    // Error if the list or url does not initalise
    assert(dlist != NULL);
    assert(url != NULL);

    // Creat a new dlist node to store this data
    Node *node = new_node(url);

    // Connect the node with the current first node
    node->next = dlist->head;

    if (dlist->size == 0) {
        // If dlist was empty, this new node is also the last node now
        dlist->last = node;
    } else {
        // Otherwise, it goes before the current first node
        dlist->head->prev = node;
    }

    // Place this new node at the front of the dlist
    dlist->head = node;

    // Update the dlist size
    dlist->size++;
}


/**
 * @brief  Remove and return the last UrlInfo data from a Dlist, 
 *         and free the memory of the node
 * 
 * @param  dlist  a Dlist
 * @return        the last UrlInfo data from a Dlist
 */
UrlInfo *dlist_remove_end(Dlist *dlist) {

    // Error if the list does not initalise or it is empty
    assert(dlist != NULL);
    assert(dlist->size > 0);

    // Set a UrlInfo to save the data will be poped
    UrlInfo *url = dlist->last->url;

    if (dlist->size == 1) {
        // If we're removing the last node, the head also needs clearing
        dlist->head = NULL;
    } else {
        // Otherwise, the previous node becomes the last node
        dlist->last = dlist->last->prev;
        free(dlist->last->prev);
        dlist->last->next = NULL;
    }

    // Update the dlist size
    dlist->size--;

    // Return the last UrlInfo data from the dlist
    return url;
}


/**
 * @brief  Remove and return the first UrlInfo data from a Dlist, 
 *         and free the memory of the node
 * 
 * @param  dlist  a Dlist
 * @return        the first UrlInfo data from a Dlist
 */
UrlInfo *dlist_remove_start(Dlist *dlist) {

    // Error if the list does not initalise or it is empty
    assert(dlist != NULL);
    assert(dlist->size > 0);

    // Set a UrlInfo to save the data will be removed
    UrlInfo *url = dlist->head->url;

    if (dlist->size == 1) {
        // If it was the last node in the dlist, the last needs to be cleared
        // and free the memory
        free(dlist->head);
        dlist->last = NULL;
    } else {
        // Otherwise, the next node becomes the first node
        dlist->head = dlist->head->next;

        // free the memory
        free(dlist->head->prev);
        dlist->head->prev = NULL;
    }

    // Update the dlist size
    dlist->size--;

    // Return the first UrlInfo data from the dlist
    return url;
}


/**
 * @brief  Get the number of elements in a Dlist
 * 
 * @param  dlist  a Dlist
 * @return        the number of elements in a Dlist
 */
int get_dlist_size(Dlist *dlist) {

    // Error if the list does not initalise
    assert(dlist != NULL);

    // Return the number of Points in a Dlist
    return dlist->size;
}


/**
 * @brief  Get the data of a given index UrlInfo from a Dlist
 * 
 * @param  dlist  a Dlist
 * @param  index  an index
 * @return        the UrlInfo data of a given index from a Dlist
 */
UrlInfo *get_dlist_point(Dlist *dlist, int index) {
    int start;
    int i;
    Node *curr;

    // Error if the list does not initalise or it is empty, or index is invalid
    assert(dlist != NULL);
    assert(dlist->size > index);
    assert(index >= 0);

    if (index >= (dlist->size / 2)) {
        // If the index is near the end, find the node from the end of the dlist
        start = dlist->size - 1;
        curr  = dlist->last;

        for (i = start; i > index; i--) {
            curr = curr->prev;
        }
    } else {
        // Otherwise, find the node from the start of the dlist
        start = 0;
        curr  = dlist->head;

        for (i = start; i < index; i++) {
            curr = curr->next;
        }
    }

    // Return the UrlInfo data of a given index from a Dlist
    return curr->url;
}


// ============================================================================
// == | Auxillary Functions 
// ============================================================================
/**
 * @brief  Create a new node and return its address
 * 
 * @param  url    a UrlInfo data
 * @return        the address of a new node, with url as data
 */
Node *new_node(UrlInfo *url) {
    
    Node *node = (Node *)malloc(sizeof(*node));
    if (node == NULL) {
        fprintf(stderr, "Error: new_node() malloc returned NULL\n");
        exit(EXIT_FAILURE);
    }


    // Assign the data value
    // Initialise the previous and next node as NULL
    node->url  = url;
    node->prev = NULL;
    node->next = NULL;

    // Return the new node address
    return node;
}


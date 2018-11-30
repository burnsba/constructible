/*
* Very simple linked list.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "global.h"

/*
* Prepend a new node to the list. Head is updated to the new node.
* This allocates memory.
*
* @head: Double pointer to head of list.
* @data: Pointer to data of the new node.
* @mem_size: Parameter passed to malloc, this is the size of 
*     the new node including whatever is necessary for data.
*
* returns: void
*/
void single_linked_list_add(single_linked_list_t** head, void* data, size_t mem_size) {
    single_linked_list_t* new_node;
    new_node = malloc(mem_size);
    
    global_exit_if_null(new_node, "Fatal error calling malloc for single_linked_list_t.\n");
    
    memset(new_node, 0, mem_size);

    new_node->data = data;
    new_node->next = *head;
    
    if (new_node->next != NULL) {
        new_node->index = new_node->next->index + 1;
    }
    
    *head = new_node;
}

/*
* Remove the head node and free the memory.
* Head is updated to the next node.
*
* @head: Double pointer to head of list.
*
* returns: the number of nodes removed (0 or 1).
*/
int single_linked_list_remove(single_linked_list_t** head) {
    single_linked_list_t* next_node = NULL;

    if (*head == NULL) {
        return 0;
    }

    next_node = (*head)->next;
    free(*head);
    *head = next_node;

    return 1;
}
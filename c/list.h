/*
* Very simple linked list.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
#ifndef __LIST_H__
#define __LIST_H__

typedef struct list {
    // data will be void* to make this somewhat generic
    void* data;
    
    struct list* next;
    
    // Since items are only inserted at the head,
    // index is also the number of items remaining,
    // which is the length of the list.
    size_t index;
} list_t;

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
void list_add(list_t** head, void* data, size_t mem_size);

/*
* Remove the head node and free the memory.
* Head is updated to the next node.
*
* @head: Double pointer to head of list.
*
* returns: the number of nodes removed (0 or 1).
*/
int list_remove(list_t** head);

#endif
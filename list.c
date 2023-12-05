#include <stdio.h>
#include <stdlib.h>
#include "list.h"

node_t *node_alloc(block_t *blk) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    if (node == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for node\n");
        exit(EXIT_FAILURE);
    }
    node->blk = blk;
    node->next = NULL;
    return node;
}

list_t *list_alloc() {
    list_t *list = (list_t *)malloc(sizeof(list_t));
    if (list == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for list\n");
        exit(EXIT_FAILURE);
    }
    list->head = NULL;
    return list;
}

void list_free(list_t *l) {
    node_t *current = l->head;
    while (current != NULL) {
        node_t *next = current->next;
        node_free(current);
        current = next;
    }
    free(l);
}

void node_free(node_t *node) {
    free(node);
}

void list_add_ascending_by_address(list_t *l, block_t *blk) {
    node_t *newNode = node_alloc(blk);
    if (l->head == NULL || l->head->blk->start > blk->start) {
        newNode->next = l->head;
        l->head = newNode;
    } else {
        node_t *current = l->head;
        while (current->next != NULL && current->next->blk->start < blk->start) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void list_remove_at_index(list_t *l, int index) {
    if (l->head == NULL) {
        fprintf(stderr, "Error: List is empty\n");
        exit(EXIT_FAILURE);
    }

    if (index == 0) {
        node_t *temp = l->head;
        l->head = l->head->next;
        node_free(temp);
    } else {
        node_t *current = l->head;
        for (int i = 0; i < index - 1 && current->next != NULL; i++) {
            current = current->next;
        }

        if (current->next == NULL) {
            fprintf(stderr, "Error: Index out of bounds\n");
            exit(EXIT_FAILURE);
        }

        node_t *temp = current->next;
        current->next = current->next->next;
        node_free(temp);
    }
}

void list_add_ascending_by_blocksize(list_t *l, block_t *blk) {
    node_t *newNode = node_alloc(blk);
    if (l->head == NULL || l->head->blk->end - l->head->blk->start + 1 > blk->end - blk->start + 1) {
        newNode->next = l->head;
        l->head = newNode;
    } else {
        node_t *current = l->head;
        while (current->next != NULL &&
               current->next->blk->end - current->next->blk->start + 1 < blk->end - blk->start + 1) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void list_add_descending_by_blocksize(list_t *l, block_t *blk) {
    node_t *newNode = node_alloc(blk);
    if (l->head == NULL || l->head->blk->end - l->head->blk->start + 1 < blk->end - blk->start + 1) {
        newNode->next = l->head;
        l->head = newNode;
    } else {
        node_t *current = l->head;
        while (current->next != NULL &&
               current->next->blk->end - current->next->blk->start + 1 > blk->end - blk->start + 1) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}
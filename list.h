#ifndef LIST_H
#define LIST_H

typedef struct block {
    int pid;   // process id
    int start; // start of the memory partition
    int end;   // end of the memory partition
} block_t;

typedef struct node {
    block_t *blk;      // pointer to the block
    struct node *next; // pointer to next node
} node_t;

typedef struct {
    node_t *head; // head of the list
} list_t;

node_t *node_alloc(block_t *blk);

list_t *list_alloc();

void list_free(list_t *l);

void list_print(list_t *l);

int list_length(list_t *l);

void list_add_to_back(list_t *l, block_t *blk);

void list_add_to_front(list_t *l, block_t *blk);

void list_add_at_index(list_t *l, block_t *blk, int index);

void list_add_ascending_by_address(list_t *l, block_t *blk);

void list_remove_at_index(list_t *l, int index);

void list_add_ascending_by_blocksize(list_t *l, block_t *blk);

void list_add_descending_by_blocksize(list_t *l, block_t *blk);

#endif /* LIST_H */
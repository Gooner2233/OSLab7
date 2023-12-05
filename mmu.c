// mmu.c

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"
#include "util.h"

void TOUPPER(char *arr) {
    for (int i = 0; i < strlen(arr); i++) {
        arr[i] = toupper(arr[i]);
    }
}

void get_input(char *args[], int input[][2], int *n, int *size, int *policy) {
    FILE *input_file = fopen(args[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        exit(0);
    }

    parse_file(input_file, input, n, size);

    fclose(input_file);

    TOUPPER(args[2]);

    if ((strcmp(args[2], "-F") == 0) || (strcmp(args[2], "-FIFO") == 0))
        *policy = 1;
    else if ((strcmp(args[2], "-B") == 0) || (strcmp(args[2], "-BESTFIT") == 0))
        *policy = 2;
    else if ((strcmp(args[2], "-W") == 0) || (strcmp(args[2], "-WORSTFIT") == 0))
        *policy = 3;
    else {
        printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
        exit(1);
    }
}

void allocate_memory(list_t *freelist, list_t *alloclist, int pid, int blocksize, int policy) {
    node_t *current = freelist->head;
    bool allocated = false;

    while (current != NULL && !allocated) {
        block_t *currentBlock = current->blk;
        int blockSize = currentBlock->end - currentBlock->start + 1;

        if (blockSize >= blocksize) {
            // Allocate the memory block
            block_t *allocatedBlock = malloc(sizeof(block_t));
            allocatedBlock->pid = pid;
            allocatedBlock->start = currentBlock->start;
            allocatedBlock->end = currentBlock->start + blocksize - 1;
            list_add_ascending_by_address(alloclist, allocatedBlock);

            // Update the remaining free block (fragment)
            if (blockSize > blocksize) {
                block_t *fragment = malloc(sizeof(block_t));
                fragment->pid = 0;
                fragment->start = allocatedBlock->end + 1;
                fragment->end = currentBlock->end;
                list_add_ascending_by_address(freelist, fragment);
            }

            // Remove the allocated block from the free list
            list_remove_at_index(freelist, 0);

            allocated = true;
        }

        current = current->next;
    }

    if (!allocated) {
        printf("Error: Not Enough Memory\n");
    }
}

void deallocate_memory(list_t *alloclist, list_t *freelist, int pid, int policy) {
    node_t *current = alloclist->head;
    bool deallocated = false;

    while (current != NULL && !deallocated) {
        block_t *currentBlock = current->blk;

        if (currentBlock->pid == pid) {
            // Deallocate the memory block
            currentBlock->pid = 0;

            // Add the block back to the free list based on policy
            if (policy == 1) {  // FIFO
                list_add_to_back(freelist, currentBlock);
            } else if (policy == 2) {  // BESTFIT
                list_add_ascending_by_blocksize(freelist, currentBlock);
            } else if (policy == 3) {  // WORSTFIT
                list_add_descending_by_blocksize(freelist, currentBlock);
            }

            // Remove the deallocated block from the allocated list
            list_remove_at_index(alloclist, 0);

            deallocated = true;
        }

        current = current->next;
    }

    if (!deallocated) {
        printf("Error: Can't locate Memory Used by PID: %d\n", pid);
    }
}

void list_coalese_nodes(list_t *l) {
    node_t *current = l->head;

    while (current != NULL) {
        block_t *currentBlock = current->blk;
        int currentEnd = currentBlock->end;

        while (current->next != NULL && currentEnd + 1 == current->next->blk->start) {
            // Merge adjacent nodes
            currentEnd = current->next->blk->end;
            node_t *temp = current->next;
            current->next = current->next->next;
            free(temp);
        }

        current = current->next;
    }
}

void print_list(list_t *list, char *message) {
    node_t *current = list->head;
    block_t *blk;
    int i = 0;

    printf("%s:\n", message);

    while (current != NULL) {
        blk = current->blk;
        printf("Block %d:\t START: %d\t END: %d", i, blk->start, blk->end);

        if (blk->pid != 0)
            printf("\t PID: %d\n", blk->pid);
        else
            printf("\n");

        current = current->next;
        i += 1;
    }
}

int main(int argc, char *argv[]) {
    int PARTITION_SIZE, inputdata[200][2], N = 0, POLICY;
    list_t *freelist, *alloclist;

    if (argc != 3) {
        fprintf(stderr, "Usage: ./mmu <input file> -{F | B | W } \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
        fflush(stdout);
        exit(1);
    }

    get_input(argv, inputdata, &N, &PARTITION_SIZE, &POLICY);

    // Initialize free and allocated lists
    freelist = list_alloc();
    alloclist = list_alloc();

    // Create the initial free block
    block_t *initial_block = malloc(sizeof(block_t));
    initial_block->start = 0;
    initial_block->end = PARTITION_SIZE - 1;
    initial_block->pid = 0;

    list_add_to_back(freelist, initial_block);

    for (int i = 0; i < N; i++) {
        int pid = inputdata[i][0];
        int blocksize = inputdata[i][1];

        if (pid > 0) {
            // Allocate memory
            allocate_memory(freelist, alloclist, pid, blocksize, POLICY);
        } else {
            // Deallocate memory
            deallocate_memory(alloclist, freelist, -pid, POLICY);

            // Coalesce free blocks
            list_coalese_nodes(freelist);
        }

        // Print free and allocated lists
        print_list(freelist, "Free List");
        print_list(alloclist, "Allocated List");
        printf("\n");
    }

    // Clean up allocated memory
    list_free(freelist);
    list_free(alloclist);

    return 0;
}
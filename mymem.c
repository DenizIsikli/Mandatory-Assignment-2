#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>
#include <stdbool.h>

/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
    // doubly-linked list
    struct memoryList *prev;
    struct memoryList *next;

    int size;            // How many bytes in this block?
    char alloc;          // 1 if this block is allocated,
    // 0 if this block is free.
    void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memoryList *head;
static struct memoryList *next_fit;

/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
    myStrategy = strategy;

    /* all implementations will need an actual block of memory to use */
    mySize = sz;

    if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

    /* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
    /*
     * Creating a new list
     * For each memory that isn't NULL, will be freed by going to the "next" in the list and free the "prev" in the list
    */
    if(head != NULL) {
        struct memoryList *trav;
        for(trav = head; trav->next != NULL; trav = trav->next) {
            free(trav->prev);
        }
        free(trav);
    }
    myMemory = malloc(sz);

    /* TODO: Initialize memory management structure. */
    head = malloc(sizeof(struct memoryList));
    next_fit = head;
    head->size = sz;
    head->ptr = myMemory;
    head->alloc = '0';
    head->prev = head->next = NULL;
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested)
{
    size_t req = requested;
    assert((int)myStrategy > 0);

    struct memoryList *trav, *temp, *best_node;
    int best_node_size;

    switch (myStrategy)
    {
        case NotSet:
            return NULL;
        case First:
            trav = head;
            while((trav->alloc == '1') || (trav->size < req)) {
                trav = trav->next;
//                if(trav == NULL) {
//                    return NULL;
//                }
            }

            break;

        case Best:
            trav = head;
            //Assigning two variables to update for the new best size and node
            best_node_size = 0;
            best_node = NULL;

            while(trav != NULL) {
                if((trav->size >= req) && (trav->alloc == '0') && (best_node_size == 0) || (trav->size < best_node_size)) {
                    best_node_size = trav->size;
                    best_node = trav;
                }
                trav = trav->next;
            }

            //Reassigning trav to best_node since best_node was assigned to trav in above code (keeping the code below identical for each strategy)
            trav = best_node;

            break;

        case Worst:
            trav = head;
            //Assigning two variables to update for the  new best size and node
            best_node_size = 0;
            best_node = NULL;

            while(trav != NULL) {
                if((trav->alloc == '0') && (trav->size > best_node_size)) {
                    best_node_size = trav->size;
                    best_node = trav;
                }
                trav = trav->next;
            }

            //Reassigning trav to best_node since best_node was assigned to trav in above code (keeping the code below identical for each strategy)
            trav = best_node;

            break;

        case Next:
            trav = next_fit;

            while(trav != NULL) {
                if ((trav->alloc == '0') && (trav->size >= req)) {
                    trav = trav->next;
                }
            }
            if(trav == NULL) {
                trav = head;
                while(trav != next_fit) {
                    if ((trav->alloc == '0') && (trav->size > req)) {
                        trav = trav->next;
                    }
                }
                if(trav == next_fit) {
                    return NULL;
                }
            }

            break;
    }

    if(trav->size > req) {
        temp = malloc(sizeof(struct memoryList));
        temp->prev = trav; //Assigns the current node to the previous node of temp list
        temp->next = trav->next; //Assigns the next node of trav list to the next node of temp list
        if(trav->next != NULL) {
            trav->next->prev = temp;
        }
        trav->next = temp;

        temp->size = trav->size - req;
        temp->ptr = trav->ptr + req;

        temp->alloc = '0';
        trav->size = req;
    }

    trav->alloc = '1';

    return trav->ptr;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
    void* ptr = block;
    struct memoryList *trav = head, *temp;

    //Check does block exist
    while(trav->ptr != ptr) {
        trav = trav->next;
        if(trav == NULL) {
            return;
        }
    }

    trav->alloc = '0';

    if ((trav->prev != NULL) && (trav->prev->alloc == '0')) { //Checking whether the previous node is null or if memory has been allocated, since the node will be used in merging
        trav->prev->size += trav->size; //Adding the size of the current node to the previous node, since the current node will be merged
        temp = trav;
        if (trav->next != NULL) {
            trav->next->prev = trav->prev;
        }
        trav->prev->next = trav->next;
        trav = trav->prev;
        free(temp);
    }
    if ((trav->next != NULL) && (trav->next->alloc == '0')) { //Checking whether the next node is null or if memory has been allocated, since the node will be used in merging
        trav->size += trav->next->size; //Adding the size of the next node to the current node, since the next node will be merged
        temp = trav->next;
        if (trav->next->next != NULL) {
            trav->next->next->prev = trav;
        }
        trav->next = trav->next->next;
        free(temp);
    }
}


/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the
 * memory pool this module manages via initmem/mymalloc/myfree.
 */

/* Get the number of contiguous areas of free space in memory. */
/*
 * Creating an Integer for the free spaces
 * Creating a new list
 * If the block is not allocated, add one to free_space
 * Go to the next element in the list
 * Return the amount of free spaces
 */
int mem_holes()
{
    struct memoryList *trav = head;
    int free_space = 0;

    while(trav != NULL) {
        if (trav->alloc == '0') {
            free_space++;
        }
        trav = trav->next;
    }
    return free_space;
}

/* Get the number of bytes allocated */
/*
 * Creating an Integer for the allocated bytes
 * Creating a new list
 * If the block is allocated, add one to alloced_byt
 * Go to the next element in the list
 * Return the amount of allocated bytes
 */
int mem_allocated()
{
    struct memoryList *trav = head;
    int alloced_byte = 0;

    while(trav != NULL) {
        if (trav->alloc == '1') {
            alloced_byte += trav->size;
        }
        trav = trav->next;
    }
    return alloced_byte;
}

/* Number of non-allocated bytes */
/*
 * Creating an Integer for the non allocated bytes
 * Creating a new list
 * If the block is allocated, add the size of the current element (in bytes) to non_alloced_byt
 * Go to the next element in the list
 * Return the amount of non allocated bytes
 */
int mem_free()
{
    struct memoryList *trav = head;
    int non_alloced_byte = 0;

    while(trav != NULL) {
        if (trav->alloc == '0') {
            non_alloced_byte += trav->size;
        }
        trav = trav->next;
    }
    return non_alloced_byte;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
/*
 * Creating an Integer for the max byte
 * Creating a new list
 * If the block is not allocated and the size of the current element is bigger than the max byte
 * Add the size to max_byt
 * Go to the next element in the list
 * Return the amount of non allocated bytes
 */
int mem_largest_free()
{
    struct memoryList *trav = head;
    int max_byte = 0;

    while(trav != NULL) {
        if (trav->alloc == '0' && trav->size > max_byte) {
            max_byte = trav->size;
        }
        trav = trav->next;
    }
    return max_byte;
}

/* Number of free blocks smaller than "size" bytes. */
/*
 * Creating an Integer for the non allocated blocks that are smaller than the size parameter
 * Creating a new list
 * If the block is not allocated and the size of the current element is smaller than the size parameter
 * Add 1 to non_alloced_blocks_smaller
 * Go to the next element in the list
 * Return the amount of non allocated blocks that are smaller than or equal to size parameter
 */
int mem_small_free(int size)
{
    struct memoryList *trav = head;
    int non_alloced_blocks_smaller = 0;

    while(trav != NULL) {
        if (trav->alloc == '0' && trav->size <= size) {
            non_alloced_blocks_smaller++;
        }
        trav = trav->next;
    }

    return non_alloced_blocks_smaller;
}

char mem_is_alloc(void *ptr)
{
    return 0;
}

/*
 * Feel free to use these functions, but do not modify them.
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
    return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
    return mySize;
}


// Get string name for a strategy.
char *strategy_name(strategies strategy)
{
    switch (strategy)
    {
        case Best:
            return "best";
        case Worst:
            return "worst";
        case First:
            return "first";
        case Next:
            return "next";
        default:
            return "unknown";
    }
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
    if (!strcmp(strategy,"best"))
    {
        return Best;
    }
    else if (!strcmp(strategy,"worst"))
    {
        return Worst;
    }
    else if (!strcmp(strategy,"first"))
    {
        return First;
    }
    else if (!strcmp(strategy,"next"))
    {
        return Next;
    }
    else
    {
        return 0;
    }
}


/*
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{

}

/* Use this function to track memory allocation performance.
 * This function does not depend on your implementation,
 * but on the functions you wrote above.
 */
void print_memory_status()
{
    printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
    printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
    printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
    strategies strat;
    void *a, *b, *c, *d, *e;
    if(argc > 1)
        strat = strategyFromString(argv[1]);
    else
        strat = First;


    /* A simple example.
       Each algorithm should produce a different layout. */

    initmem(strat,500);
    a = mymalloc(100);
    b = mymalloc(100);
    c = mymalloc(100);
    myfree(b);
    d = mymalloc(50);
    myfree(a);
    e = mymalloc(25);

    print_memory();
    print_memory_status();
}

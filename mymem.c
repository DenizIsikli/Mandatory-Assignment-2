#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memoryList
{
  // doubly-linked list
  struct memoryList *last;
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
static struct memoryList *next;


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
     * For each memory that isn't NULL, will be freed by going to the "next" in the list and free the "last" in the list
    */
    //struct memoryList *head;
    if(head != NULL) {
        struct memoryList *trav;
        for (trav = head; trav->next != NULL; trav = trav->next) {
            free(trav->last);
        }
        free(trav);
    }
	myMemory = malloc(sz);
	
	/* TODO: Initialize memory management structure. */
    head = malloc(sizeof(struct memoryList));
    head->size = sz;
    head->alloc = 0;
    head->ptr = myMemory;
    head->last = head->next = NULL;
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

    struct memoryList *trav = head, *temp;
	
	switch (myStrategy)
	  {
	  case NotSet: 
	            return NULL;
	  case First:
          while(trav->size < req || trav->alloc != 1) {
              trav = trav->next;
          }


          if(trav->next > req) {
              temp = malloc(sizeof(struct memoryList));
              temp->last = trav; //explain line
              temp->next = trav->next; //explain line
              if(trav->next != NULL) {
                  trav->next->last = temp; //explain line
              }
              trav->next = temp;

              temp->size = trav->size - req;
              temp->ptr = trav->ptr + req;

              temp->alloc = 0; //why should temp contain 0 - comment
              trav->alloc = 1; //why should trav contain 1 - comment
              trav->size = req;
          }
          return trav->ptr;
	  case Best:
	            return NULL;
	  case Worst:
	            return NULL;
	  case Next:
	            return NULL;
	  }
	return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
    struct memoryList *trav = head, *temp;

    if(trav == NULL) {
        return;
    } else {
        while (trav->ptr != block) {
            trav = trav->next;
        }
        trav->alloc = 0;

        //The line belows checks if the node behind is not NULL, and not allocated (free of space)
        if((trav->last != NULL) && (trav->last->alloc == 0)) {

            //Sets the size of the last node equal to the size of the current node
            trav->last->size += trav->size;
            temp = trav;
            //Question: Correct or not?
            if(trav->next != NULL) {
                trav->next->last = trav->last;
            }
            free(temp);
        }
        //The line belows checks if the node in front is not NULL, and not allocated (free of space)
        if((trav->next != NULL) && (trav->next->alloc == 0)) {

            //Sets the size of the last node equal to the size of the current node
            trav->last->size += trav->size;
            temp = trav->next;
            //Question: Correct or not
            if(trav->next->next != NULL) {
                trav->next->next->last = trav;
            }
            free(temp);
        }
    }
	return;
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
    int free_space = 0;
    struct memoryList *trav = head;

    while(trav != NULL) {
        if (trav->alloc == 0) {
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
    int alloced_byte = 0;
    struct memoryList *trav = head;

    while(trav != NULL) {
        if (trav->alloc == 1) {
            alloced_byte++;
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
    int non_alloced_byte = 0;
    struct memoryList *trav = head;

    while(trav != NULL) {
        if (trav->alloc == 1) {
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
    int max_byte = 0;
    struct memoryList *trav = head;

    while(trav != NULL) {
        if (trav->alloc == 0 && trav->size > max_byte) {
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
    int non_alloced_blocks_smaller = 0;
    struct memoryList *trav = head;

    while(trav != NULL) {
        if (trav->alloc == 0 && trav->size <= size) {
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
    struct memoryList *trav;

    for(trav = head; trav < trav->next != NULL; trav = trav->next) {
        printf("Bytes: %d", trav->size);
    }
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

//mypthread.c
//Christina Segerholm cms549
//Athira Haridas ah671
#ifndef H_MYPTHREAD
#define H_MYPTHREAD
#define _GNU_SOURCE

#include <ucontext.h>
#include <stdio.h>      
#include <stdlib.h>
#include <errno.h>

typedef enum {
	ACTIVE,
	BLOCKED,
	DEAD
} mypthread_st;

// Types
typedef struct {
	// Define any fields you might need inside here.
	int tid;
	ucontext_t* context;
	mypthread_st state;
	int joined;
} mypthread_t;

typedef struct {
	// Define any fields you might need inside here.
} mypthread_attr_t;

// Functions
int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
			void *(*start_routine) (void *), void *arg);

void mypthread_exit(void *retval);

int mypthread_yield(void);

int mypthread_join(mypthread_t thread, void **retval);

//Circular linkedlist for storing the threads for execution
struct node {
	struct node *prev;
	mypthread_t *node;
	struct node *next;
}*head, *tail, *temp;


/* Don't touch anything after this line.
 *
 * This is included just to make the mtsort.c program compatible
 * with both your ULT implementation as well as the system pthreads
 * implementation. The key idea is that mutexes are essentially
 * useless in a cooperative implementation, but are necessary in
 * a preemptive implementation.
 */

typedef int mypthread_mutex_t;
typedef int mypthread_mutexattr_t;

static inline int mypthread_mutex_init(mypthread_mutex_t *mutex,
			const mypthread_mutexattr_t *attr) { return 0; }

static inline int mypthread_mutex_destroy(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_lock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_trylock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_unlock(mypthread_mutex_t *mutex) { return 0; }

#endif
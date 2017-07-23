#include "ucontext.h"
#include "mypthread.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

int cTID = 1; // current internal thread id
int numTh = 1; //number of threads
int numNodes = 0; //number of nodes for circuular linked lists

mypthread_t* findTh(int tID) {
	temp = head;
	while (temp) {
		mypthread_t* t = temp->node;
		//search through the list of running threads
		if (t->tid == tID){
			//return the one that matches the current thread id
			return temp->node;
		}
		else {
			//otherwise continue looping until we find it
			temp = temp->next;
		}
	}
	//if it never returns, we didn't find it
	printf("Error: Thread %d not found!", tID);
	exit(0);
}

mypthread_t* findNextTh(int tID) {
	//First search from head to find tID
	temp = head;

	while (temp) {
		if (temp->node->tid == tID) {
			break; //break when we find the current thread
		} 
		else {
			temp = temp->next;
		}
	}
	//as long as there exists a next node
	if (temp->next) {
		temp = temp->next;
		while (temp) {
			if (temp->node->state == ACTIVE) {
				return temp->node; //if its active return it
			} 
			else { //otherwise continue looping until we find the next active thread
				temp = temp->next;
			}
		}

	} 
	//if there are no more threads, exit 
	else {
		exit(0);
	}

	printf("No active thread found after Thread ID:%d\n", tID);
	exit(0);
}

//Insert node at the end of LinkedList
void addToRobin(mypthread_t* data) {
	if (head == NULL) { //for the first node
		mypthread_t data;
		temp = (struct node *) malloc(1 * sizeof(struct node));
		temp->prev = NULL;
		temp->next = NULL;
		numNodes++;
		head = temp;
		tail = temp;
	} else {
		mypthread_t data;
		temp = (struct node *) malloc(1 * sizeof(struct node));
		temp->prev = NULL;
		temp->next = NULL;
		numNodes++;
		tail->next = temp;
		temp->prev = tail;
		tail = temp;
		tail->next = head;
		head->prev = tail;
	}
	temp->node = data;
}


int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
	if (!numNodes){ //if there are no threads, create a master one
		mypthread_t* masterTh = (mypthread_t *) malloc(sizeof(mypthread_t));
		masterTh->tid = numTh++; //set the tid
		ucontext_t* c = (ucontext_t*) malloc(sizeof(ucontext_t));
		masterTh->context = c; //set up the master threads context
		masterTh->context->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096);
		masterTh->context->uc_stack.ss_size = 4096;
		masterTh->state = ACTIVE; //set the state to active
		addToRobin(masterTh);
	}
	ucontext_t* c = (ucontext_t*) malloc(sizeof(ucontext_t));
	thread->context = c;
	getcontext(thread->context);
	(*thread).context->uc_stack.ss_sp = (char*) malloc(sizeof(char) * 4096);
	(*thread).context->uc_stack.ss_size = 4096;
	(*thread).state = ACTIVE;
	thread->tid = numTh++;
	makecontext(thread->context, (void (*)()) start_routine, 1, arg);
	addToRobin(thread);
	return 0;
}

void mypthread_exit(void *retval) {
	//get the current thread from the list of threads
	mypthread_t* currTh = findTh(cTID);
	//set the state of the current thread to dead
	currTh->state = DEAD;
	//deallocate the memory allocated to the current threads context
	free(currTh->context);

	if (currTh->joined != 0){ //if the thread was joined somewhere else
		mypthread_t* joinThread = findTh(currTh->joined); //get the thread that joined it
		//set the current thread to this one
		joinThread->state = ACTIVE;
	}
	//find the next active thread from the list
	mypthread_t* nextActiveTh = findNextTh(currTh->tid);
	//check if the this is actually the next active thread
	if (cTID == nextActiveTh->tid){//should return
		return;
	}
	//set the context to this new thread
	cTID = nextActiveTh->tid; //reset the current thread id to this
	setcontext(nextActiveTh->context); //set the current context to this thread
	return;
}

int mypthread_yield(void) {
	//find the current thread based on its id
	mypthread_t* cTh = findTh(cTID);
	//find the next active thread from round robin
	mypthread_t* next = findNextTh(cTh->tid);
	//return 0 if the next thread is the current thread
	if (cTID == next->tid){
		return 0;
	}

	cTID = next->tid;
	swapcontext(cTh->context, next->context);
	return 0;
}

int mypthread_join(mypthread_t thread, void **retval) {
	//get the tid of the entered thread
	int switchToID = thread.tid;
	//find the current thread
	mypthread_t* curr = findTh(cTID);
	//find the entered thread
	mypthread_t* swapper = findTh(thread.tid);
	//if the state of the swapper isnt active, return 0
	if (swapper->state != ACTIVE) {
		return 0;
	} 
	//otherwise...
	else {
		//set the state of the current thread to blocked
		curr->state = BLOCKED;
		//set the joined tid for the swapper to the current thread
		swapper->joined = cTID;
		//set the current thread equal to the new thread id
		cTID = switchToID;
		//swap their contexts
		swapcontext(curr->context, swapper->context);
	}
	return 0;
}
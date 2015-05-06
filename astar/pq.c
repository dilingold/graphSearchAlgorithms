/******************/
/* PRIORITY QUEUE */
/******************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "pq.h"


/*
	Initialise the priority queue to empty.
	Return value is a pointer to a new priority queue structure.
	This should be free'd by calling pq_destroy.
*/
struct priority_queue *pq_create()
{
	/* you will need to implement this */
	struct priority_queue *pq;
	pq = malloc(sizeof(struct priority_queue));
	pq->size = 0;
	pq->heap = malloc(sizeof(struct heapnode)*LARGE_TEST_SIZE);
	if (pq->heap == NULL)
	{
		fprintf(stderr, "Memory allocation failed");
		exit(EXIT_FAILURE);
	}
	return pq;
}


/*
	Cleanup the priority queue and free any memory within.	
*/
void pq_destroy(struct priority_queue *pq)
{
	/* you will need to implement this */
	return;
}


/*
	Return the number of elements in the priority queue.
	This is needed to test the priority queue.
	returns: int, the size of the pq
*/
int pq_size(struct priority_queue *pq) {
	
	assert(pq != NULL);
	return pq->size;
}


/*
	Insert a value with attached min.prority in the priority queue.
	priority should be non-negative (>= 0)
	returns: 1 if successful, 0 if failed (pq is full)
*/
int pq_enqueue(struct priority_queue *pq, int val, int priority)
{
	/* you will need to implement this */
	struct heapnode *new_heap;
	struct heapnode *parent_heap;
	
	int position;

	assert (priority >= 0);
	
	if (pq->size == MAX_HEAP_SIZE || pq == NULL)
	{
		return 0;
	}
	if (pq->size == 0)
	{
		new_heap = &pq->heap[1];
		new_heap->val = val;
		new_heap->priority = priority;
		pq->size++;
		return 1;
	}
	pq->size++;
	position = pq->size;
	new_heap = &pq->heap[position];
	parent_heap = &pq->heap[position / 2];
	new_heap->val = val;
	new_heap->priority = priority;
	
	while (pq_cmp(pq, parent_heap->priority, new_heap->priority) > 0)
	{
		pq_swap(pq, position / 2, position);
		position = position / 2;
		new_heap = &pq->heap[position];
		parent_heap = &pq->heap[position / 2];

	}
	
	return 1;
}

/*
	Return the value with the lowest priority in the queue.
	This keeps the value stored within the priority queue.
	returns: 1 if successful, 0 if failed (pq is empty)
	val and priority are returned in the pointers
*/	
int pq_find(struct priority_queue *pq, int *val, int *priority)
{
	struct heapnode *min;
	if (pq->size == 0)
	{
		return 0;
	}
	min = &pq->heap[1];
	*val = min->val;
	*priority = min->priority;
	return 1;
}

/*
	Removes the lowest priority object from the priority queue
	returns: 1 if successful, 0 if failed (pq is empty)

	note: may remove this from the spec (do cleanup)
*/
int pq_delete(struct priority_queue *pq)
{
	struct heapnode *parent;
	struct heapnode *child;
	struct heapnode *del;
	int p_position;
	int c_position;
	int swaps;
	
	if (pq == NULL)
	{
		return 0;
	}
	p_position = 1;
	del = &pq->heap[pq->size];
	parent = &pq->heap[p_position];
	parent->val = del->val;
	parent->priority = del->priority;
	del = NULL;
	pq->size--;
	swaps = 1;
	c_position = p_position*2;
	while (c_position <= pq->size && swaps == 1)
	{
		if ((pq_cmp(pq, (&pq->heap[c_position])->priority,(&pq->heap[p_position*2+1])->priority) > 0))
		{
			c_position = p_position*2+1;
		}
		child = &pq->heap[c_position];
		if (pq_cmp(pq, parent->priority, child->priority) > 0)
		{
			pq_swap(pq, p_position, c_position);
			p_position = c_position;
			c_position = p_position*2;
			parent = &pq->heap[p_position];
		}
		else
		{
			swaps = 0;
		}
	}
	return 1;
}


/*
	Returns the value with the lowest priority and removes it
	from the queue.
	This is the same as pq_find followed by pq_delete.
	returns: 1 if successful, 0 if failed (pq is empty)
	val and priority are returned in the pointers
*/	
int pq_dequeue(struct priority_queue *pq, int *val, int *priority)
{
	int ret;
	
	if (pq->size == 0)
	{
		return 0;
	}
	ret = pq_find(pq, val, priority);
	if (ret == 0)
	{
		return 0;
	}
	ret = pq_delete(pq);
	return ret;
}

/*
	Compares two pq elements with each other.
	This is needed to test the heap contained within.
	returns: <0 if a < b, >0 if a > b, 0 if a == b
	note that the root of the heap is at index 1, not zero
*/
int pq_cmp(struct priority_queue *pq, int a, int b)
{
	if (a < b)
	{
		return -1;
	}
	else if (a > b)
	{
		return 1;
	}
	return 0;
}

/*
	Swaps two pq elements with each other.
	This is an internal function that will help with the heap.
*/
void pq_swap(struct priority_queue *pq, int a, int b)
{
	int aval;
	int aprio;
	struct heapnode *anode;
	struct heapnode *bnode;
	
	anode = &pq->heap[a];
	bnode = &pq->heap[b];
	aval = anode->val;
	aprio = anode->priority;
	anode->val = bnode->val;
	anode->priority = bnode->priority;
	bnode->val = aval;
	bnode->priority = aprio;
	
	return;
}



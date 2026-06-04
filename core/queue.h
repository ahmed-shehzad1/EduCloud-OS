#ifndef QUEUE_H
#define QUEUE_H

#include "job.h"

/*
===========================================
MODULE: Queue
PURPOSE: Linked list based job queue
===========================================
*/

typedef struct QueueNode
{
    Job job;
    struct QueueNode* next;
} QueueNode;

typedef struct
{
    QueueNode* front;
    QueueNode* rear;
    int size;
} JobQueue;

/* Initializes queue */
void initializeQueue(JobQueue* queue);

/* Add job to queue */
void enqueue(JobQueue* queue, Job job);

/* Remove job from queue */
Job dequeue(JobQueue* queue);

/* Check if empty */
int isEmpty(JobQueue* queue);

/* Display queue */
void displayQueue(JobQueue* queue);

#endif
/*
===========================================
MODULE: Queue
PURPOSE: Linked list implementation of job queue
===========================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/* Initialize queue */
void initializeQueue(JobQueue* queue)
{
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

/* Check if queue is empty */
int isEmpty(JobQueue* queue)
{
    return queue->size == 0;
}

/* Add job to queue (FIFO) */
void enqueue(JobQueue* queue, Job job)
{
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));

    if (newNode == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    newNode->job = job;
    newNode->next = NULL;

    if (queue->rear == NULL)
    {
        queue->front = queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    queue->size++;
}

/* Remove job from queue */
Job dequeue(JobQueue* queue)
{
    Job emptyJob;
    emptyJob.id = -1;
    strcpy(emptyJob.name, "EMPTY");
    emptyJob.priority = -1;
    emptyJob.status = JOB_FAILED;

    if (isEmpty(queue))
    {
        printf("Queue is empty!\n");
        return emptyJob;
    }

    QueueNode* temp = queue->front;
    Job removed = temp->job;

    queue->front = queue->front->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(temp);
    queue->size--;

    return removed;
}

/* Display queue */
void displayQueue(JobQueue* queue)
{
    if (isEmpty(queue))
    {
        printf("\nQueue is empty.\n");
        return;
    }

    QueueNode* current = queue->front;

    printf("\n========= JOB QUEUE =========\n");

    while (current != NULL)
    {
        printf("ID: %d | Name: %s | Priority: %d | Status: %d\n",
               current->job.id,
               current->job.name,
               current->job.priority,
               current->job.status);

        current = current->next;
    }

    printf("=============================\n");
}
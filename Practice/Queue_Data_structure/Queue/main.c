#include <stdio.h>
#include <stdlib.h>

typedef struct que{
    int capacity;
    int front, rear, *item;
    int count;
}queue_t;

#define success 0
#define failure -1
int create_queue(queue_t *q, int size)
{
    q->item = malloc(sizeof(int)*size);
    if(q->item == NULL)
    {
        return failure;
    }
    q->front = -1;
    q->rear = -1;
    q->capacity = size;
    q->count = 0;
    return success;
}
int enqueue(queue_t *q, int data)
{
    if(q->count == q->capacity)
    {
        return failure;
    }
    if(q->front == -1)
    {
        q->front = 0;
    }
    q->rear = (q->rear + 1) % (q->capacity);
    q->item[q->rear] = data;
    q->count++;
    return success;
}
int print_queue(queue_t q)
{
    if(q.count == 0)
    {
        return failure;
    }
    printf("Front > ");
    while(q.count > 0)
    {
        printf("%d ", q.item[q.front]);
        q.front = (q.front +1) % (q.capacity);
        q.count--;
    }
    printf(" < rear");
    printf("\n");
    return success;
}
int dequeue(queue_t *q, int *top)
{
    if(q->count == 0)
    {
        return failure;
    }
    *top = q->item[q->front];
    q->front = (q->front + 1) % (q->capacity);
    (q->count)--;
    return failure;
}
int main()
{
    int size = 5;
    queue_t q;
    create_queue(&q, size);
    enqueue(&q, 50);
    enqueue(&q, 40);
    enqueue(&q, 30);
    enqueue(&q, 20);
    enqueue(&q, 10);
    print_queue(q);
    int top ;
    dequeue(&q, &top);
    printf("depueued : %d\n", top);
    print_queue(q);
}

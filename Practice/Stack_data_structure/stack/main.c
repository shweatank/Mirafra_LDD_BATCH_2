#include <stdio.h>
#include <stdlib.h>

typedef struct stack{
    int capacity;
    int top;
    int *stack;
}stack_t;

#define success 0
#define failure -1

int is_stack_full(stack_t *s)
{
    if(s->top == s->capacity - 1)
    {
        return failure; //stack is full
    }
    else
    {
        return success;
    }
}
int is_stack_empty(stack_t *s)
{
    if(s->top == -1)
    {
        return failure;  //stack is empty
    }
    else
    {
        return success; 
    }
}
int create_stack(stack_t *s, int size)
{
    s->stack = malloc(sizeof(int)*size);
    if(s->stack == NULL)
    {
        return failure;
    }
    s->top = -1;
    s->capacity = size;
    return success;
}
int push(stack_t *s, int data)
{
    if(is_stack_full(s))
    {
        return failure;
    }
    ++s->top;
    s->stack[s->top] = data;
    return success;
}

int peep(stack_t s)
{
    if(s.top == -1)
    {
        return failure;
    }
    
    printf("Stack Top -> ");
    while(s.top > -1)
    {
        printf("%d ", s.stack[(s.top)--]);
    }
    printf("\n");
}
int pop(stack_t *s, int *element)
{
    if(is_stack_empty(s))
    {
        return failure;
    }
    (s->top)--;  // decreamenting the top value (deleting top)
    *element = s->stack[s->top]; //updating and retrieving next top to element variable
    return success;
    
}
int main()
{
    int size = 5; //stack size
    stack_t s; //stack structure variable
    create_stack(&s, size);
    
    push(&s, 10);
    push(&s, 20);
    push(&s, 30);
    push(&s, 40);
    push(&s, 50);
    peep(s);
    
    int top_element;
    pop(&s, &top_element);
    printf("the top element in the stack = %d\n", top_element);
    peep(s);
}


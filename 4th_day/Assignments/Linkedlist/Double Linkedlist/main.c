#include "main.h"

int main() {
    dlist *head = NULL, *tail = NULL;
    insert_at_last(&head, &tail, 10);
    insert_at_last(&head, &tail, 20);
    insert_at_last(&head, &tail, 30);
    insert_at_last(&head, &tail, 40);
    insert_at_last(&head, &tail, 50);

    print_list(head);
    delete_node(&head,&tail, 50);
   
    print_list(head);
 
      delete_last(&head, &tail);
     print_list(head);

    return 0;
}
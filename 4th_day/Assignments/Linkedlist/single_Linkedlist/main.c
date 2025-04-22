#include "main.h"
#include <stdio.h>

int main()
{
    slist *head = NULL; // Fix: single pointer
    int option, data;

    while (1)
    {
        printf("\nChoose an operation:\n");
        printf("1. Insert at first\n2. Insert at last\n3. Print list\n");
        printf("4. Delete at first\n5. Delete at last\n6. Delete list\n7. Exit\n");
        printf("Enter option: ");
        scanf("%d", &option);

        switch (option)
        {
            case 1:
                printf("Enter the data: ");
                scanf("%d", &data);
                if (insert_at_first(&head, data) != success)
                {
                    printf("Insert at first failed.\n");
                }
                break;

            case 2:
                printf("Enter the data: ");
                scanf("%d", &data);
                if (insert_at_last(&head, data) != success)
                {
                    printf("Insert at last failed.\n");
                }
                break;

            case 3:
                if (printlist(head) != success)
                {
                    printf("Print list failed.\n");
                }
                break;

            case 4:
                if (Delete_at_first(&head) != success)
                {
                    printf("Delete at first failed.\n");
                }
                break;

            case 5:
                if (Delete_at_last(&head) != success)
                {
                    printf("Delete at last failed.\n");
                }
                break;

            case 6:
                if (Delete_list(head) != success)
                {
                    printf("Delete list failed.\n");
                }
                break;

            case 7:
                printf("Exiting...\n");
                return 0;

            default:
                printf("Invalid option. Try again.\n");
        }
    }

    return 0;
}

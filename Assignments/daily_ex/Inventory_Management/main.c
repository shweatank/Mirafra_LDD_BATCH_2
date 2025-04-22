#include <stdio.h>
#include <string.h>

struct Product {
    char name[50];
    int id;
    int quantity;
    float price;
};

int main() {
    int n;
    printf("how many products to start with? ");
    scanf("%d", &n);
    getchar();

    struct Product inventory[n + 1];
    int currentCount = n;

    for (int i = 0; i < n; i++) {
        printf("\nproduct %d details:\n", i + 1);
        printf("Name: ");
        fgets(inventory[i].name, 50, stdin);
        inventory[i].name[strcspn(inventory[i].name, "\n")] = 0;

        printf("ID: ");
        scanf("%d", &inventory[i].id);

        printf("Qty: ");
        scanf("%d", &inventory[i].quantity);

        printf("Price: ");
        scanf("%f", &inventory[i].price);
        getchar();
    }

    printf("\nadd a new product:\n");
    printf("Name: ");
    fgets(inventory[currentCount].name, 50, stdin);
    inventory[currentCount].name[strcspn(inventory[currentCount].name, "\n")] = 0;

    printf("ID: ");
    scanf("%d", &inventory[currentCount].id);

    printf("Qty: ");
    scanf("%d", &inventory[currentCount].quantity);

    printf("Price: ");
    scanf("%f", &inventory[currentCount].price);
    currentCount++;

    int threshold;
    printf("\nenter qty threshold: ");
    scanf("%d", &threshold);

    printf("\nproducts with qty below %d:\n", threshold);
    for (int i = 0; i < currentCount; i++) {
        if (inventory[i].quantity < threshold) {
            printf("Name: %s, ID: %d, Qty: %d, Price: %.2f\n",
                   inventory[i].name, inventory[i].id, inventory[i].quantity, inventory[i].price);
        }
    }

    return 0;
}

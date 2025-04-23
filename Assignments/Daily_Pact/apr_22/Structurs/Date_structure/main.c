#include <stdio.h>

struct Date {
    int day;
    int month;
    int year;
};

void printDate(struct Date d) {
    printf("%02d/%02d/%d\n", d.day, d.month, d.year);
}

int main() {
    struct Date today = {22, 4, 2025};
    printDate(today);
    return 0;
}


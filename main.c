#include <stdio.h>

#include "add.h"
#include "sub.h"
#include "mul.h"
#include "div.h"

int main(void)
{
    int a = 20;
    int b = 5;
    int choice;

    printf("C Compilation Pipeline Calculator\n");
    printf("---------------------------------\n");

    printf("a = %d\n", a);
    printf("b = %d\n\n", b);

    printf("1. Addition\n");
    printf("2. Subtraction\n");
    printf("3. Multiplication\n");
    printf("4. Division\n");

    printf("\nEnter your choice: ");
    scanf("%d", &choice);

    switch (choice)
    {
        case 1:
            printf("Result: %d\n", add(a, b));
            break;

        case 2:
            printf("Result: %d\n", sub(a, b));
            break;

        case 3:
            printf("Result: %d\n", mul(a, b));
            break;

        case 4:
            printf("Result: %d\n", divide(a, b));
            break;

        default:
            printf("Invalid choice!\n");
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>

void basics() {
    int x = 10;
    int *p = &x;  // pointer to int

    printf("\n[Pointer Basics]\n");
    printf("x      = %d\n", x);
    printf("&x     = %p (address of x)\n", (void*)&x);
    printf("p      = %p (value stored in p = address of x)\n", (void*)p);
    printf("*p     = %d (value at the address p points to)\n", *p);

    *p = 20; // modify x through pointer
    printf("After *p = 20, x = %d\n", x);
}

void arrays_and_pointers() {
    int arr[5] = {1, 2, 3, 4, 5};
    int *p = arr; // same as &arr[0]

    printf("\n[Arrays and Pointers]\n");
    printf("arr      = %p (address of first element)\n", (void*)arr);
    printf("&arr[0]  = %p\n", (void*)&arr[0]);
    printf("p        = %p\n", (void*)p);

    for (int i = 0; i < 5; i++) {
        printf("*(p + %d) = %d\n", i, *(p + i));
    }
}

void pointer_arithmetic() {
    char str[] = "Hello";
    char *p = str;

    printf("\n[Pointer Arithmetic]\n");
    printf("String: %s\n", str);
    while (*p != '\0') {
        printf("p: %p, *p: %c\n", (void*)p, *p);
        p++;
    }
}

void const_pointer_examples() {
    int a = 100, b = 200;
    const int *ptr1 = &a;  // pointer to const int
    int *const ptr2 = &a;  // const pointer to int

    printf("\n[Const Pointers]\n");
    printf("ptr1 points to a = %d\n", *ptr1);
    // *ptr1 = 300; // ERROR: can't change value via ptr1
    ptr1 = &b; // OK: can point to different int

    printf("ptr2 points to a = %d\n", *ptr2);
    *ptr2 = 300; // OK: can change value
    // ptr2 = &b; // ERROR: can't change where ptr2 points
}

void dynamic_memory() {
    int n = 3;
    int *p = (int *)malloc(n * sizeof(int));

    if (!p) {
        perror("malloc failed");
        return;
    }

    printf("\n[Dynamic Memory]\n");
    for (int i = 0; i < n; i++) {
        p[i] = (i + 1) * 10; // same as *(p + i)
        printf("p[%d] = %d (address: %p)\n", i, p[i], (void*)&p[i]);
    }

    free(p);
}

int main() {
    printf("=== Pointer Playground in C ===\n");

    basics();
    arrays_and_pointers();
    pointer_arithmetic();
    const_pointer_examples();
    dynamic_memory();

    printf("\nExperiment with modifying code to see effects!\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
int global_init = 42;   // Initialized global → Data segment
int global_uninit;      // Uninitialized global → BSS segment
void print_addresses() {
    int stack_var = 123;             // Stack
    static int static_local = 456;   // Initialized static → Data segment
    int *heap_var = (int *) malloc(sizeof(int)); // Heap
    if (!heap_var) {
        perror("malloc failed");
        return;
    }
    *heap_var = 789;
    printf("=== Memory Layout ===\n");
    printf("Code (function addr) : %p\n", (void*)print_addresses);
    printf("Initialized global   : %p (global_init)\n", (void*)&global_init);
    printf("Uninitialized global : %p (global_uninit)\n", (void*)&global_uninit);
    printf("Initialized static   : %p (static_local)\n", (void*)&static_local);
    printf("Heap (malloc)        : %p (heap_var)\n", (void*)heap_var);
    printf("Stack (local var)    : %p (stack_var)\n", (void*)&stack_var);
    free(heap_var);
}
int main() {
    print_addresses();
    return 0;
}

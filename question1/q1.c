#include <stdio.h>
#include <stdlib.h>

// Variable declaration
int global_counter = 5;

// Allocates memory and fills it
int* allocate_array(int size) {
    int *arr = (int*) malloc(size * sizeof(int)); // dynamic allocation

    for (int i = 0; i < size; i++) { // loop
        arr[i] = i + global_counter; // write to heap memory
    }

    return arr;
}

// Processes array (conditional + loop)
int process_array(int *arr, int size) {
    int sum = 0;

    for (int i = 0; i < size; i++) {
        if (arr[i] % 2 == 0) { // conditional
            sum += arr[i];
        }
    }

    return sum;
}

// Prints result
void print_result(int result) {
    printf("Sum of even numbers: %d\n", result); // standard library
}

int main() {
    int size = 10;

    int *data = allocate_array(size);
    int result = process_array(data, size);
    print_result(result);

    free(data);
    return 0;
}


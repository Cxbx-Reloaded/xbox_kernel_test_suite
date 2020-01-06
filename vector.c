#include <stdlib.h>
#include <windows.h>
#include <hal/xbox.h>
#include "vector.h"
#include "output.h"

// This vector implementation is greatly inspired from a tutorial by hbs
// Ref: https://www.happybearsoftware.com/implementing-a-dynamic-array

void vector_init(vector *vec) {
    vec->size = 0;
    vec->capacity = 379;
    vec->data = malloc(sizeof(int) * vec->capacity);
}

void vector_append(vector *vec, int value) {
    vector_double_capacity_if_full(vec);
    vec->data[vec->size++] = value;
}

int vector_get(vector *vec, int index) {
    if (index >= vec->size || index < 0) {
        print("Error: vector index out of bound\n");
        Sleep(5000);
        XReboot();
    }
    return vec->data[index];
}

void vector_set(vector *vec, int index, int value) {
    // zero fill the vector up to the desired index
    while (index >= vec->size) {
        vector_append(vec, 0);
    }
    vec->data[index] = value;
}

void vector_double_capacity_if_full(vector *vec) {
    if (vec->size >= vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(int) * vec->capacity);
    }
}

void vector_free(vector *vec) {
    if(vec!=NULL)
        free(vec->data);
}

#pragma once

// This vector implementation is greatly inspired from a tutorial by hbs
// Ref: https://www.happybearsoftware.com/implementing-a-dynamic-array

typedef struct {
  int size;
  int capacity;
  int *data;
} vector;

void vector_init(vector *vec);
void vector_append(vector *vec, int value);
int vector_get(vector *vec, int index);
void vector_set(vector *vec, int index, int value);
void vector_double_capacity_if_full(vector *vec);
void vector_free(vector *vec);

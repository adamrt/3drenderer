#include <stdlib.h>
#include <stdio.h>
#include "triangle.h"

void triangle_array_init(TriangleArray *a, size_t initial_size) {
    a->triangles = malloc(initial_size * sizeof(Triangle));
    if (a->triangles == NULL) {
        fprintf(stderr, "failed to malloc for init_triangles");
        exit(1);
    }
    a->length = 0;
    a->capacity = initial_size;
}

void triangle_array_insert(TriangleArray *a, Triangle tri) {
    if (a->length == a->capacity) {
        a->capacity *= 2;
        Triangle *tmp;
        tmp = realloc(a->triangles, a->capacity * sizeof(Triangle));
        if (tmp == NULL) {
            triangle_array_free(a);
            fprintf(stderr, "failed to realloc");
            exit(1);
        }
        a->triangles = tmp;

    }
    a->triangles[a->length] = tri;
    a->length++;
}

void triangle_array_free(TriangleArray *a) {
    if (a->triangles != NULL){
        free(a->triangles);
    }
    a->triangles = NULL;
    a->length = a->capacity = 0;
}

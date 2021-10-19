#include <stdbool.h>

#include "array.h"
#include "triangle.h"

// TODO: this is bubble sort-ish. change to merge sort or something faster.
void sort_triangles_by_z(triangle_t *tris) {
    // z gets higher further into the screen (left handed);
    // higher numbers should be first
    int num_tris = array_length(tris);
    while(true) {
        bool has_changed = false;
        // use num_tris - 1 because we use i+1 for b;
        for (int i = 0; i < (num_tris - 1); i++) {
            if (tris[i].avg_depth < tris[i+1].avg_depth) {
                triangle_t tmp = tris[i];
                tris[i] = tris[i+1];
                tris[i+1]  = tmp;
                has_changed = true;
            }
        }
        if (has_changed == false) {
            break;
        }
    }
}

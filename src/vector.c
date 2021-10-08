#include <math.h>
#include "vector.h"

Vec3 vec3_rotate_x(Vec3 v, float angle) {
    Vec3 rotated_vector = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}

Vec3 vec3_rotate_y(Vec3 v, float angle) {
    Vec3 rotated_vector = {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y ,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };
    return rotated_vector;
}

Vec3 vec3_rotate_z(Vec3 v, float angle) {
    Vec3 rotated_vector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };
    return rotated_vector;
}

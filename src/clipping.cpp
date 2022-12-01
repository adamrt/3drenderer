#include <math.h>
#include <vector>

#include <glm/glm.hpp> // vec3 normalize reflect dot pow

#include "clipping.h"

static float float_lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

#define NUM_PLANES 6
plane_t frustum_planes[NUM_PLANES];

/* Frustum planes are defined by a point and a normal vector
   Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
   Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
   Top plane    :  P=(0, 0, 0),     N=(0, -cos(fovy/2), sin(fovy/2))
   Bottom plane :  P=(0, 0, 0),     N=(0, cos(fovy/2), sin(fovy/2))
   Left plane   :  P=(0, 0, 0),     N=(cos(fovx/2), 0, sin(fovx/2))
   Right plane  :  P=(0, 0, 0),     N=(-cos(fovx/2), 0, sin(fovx/2))

             /|\
           /  | |
         /\   | |
       /      | |
    P*|-->  <-|*|   ----> +z-axis
       \      | |
         \/   | |
           \  | |
             \|/
*/
void init_frustum_planes(float fov_x, float fov_y, float znear, float zfar)
{
    float cos_half_fov_x = cos(fov_x / 2);
    float sin_half_fov_x = sin(fov_x / 2);
    float cos_half_fov_y = cos(fov_y / 2);
    float sin_half_fov_y = sin(fov_y / 2);

    frustum_planes[LEFT_FRUSTUM_PLANE].point = glm::vec3(0, 0, 0);
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov_x;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = glm::vec3(0, 0, 0);
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov_x;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

    frustum_planes[TOP_FRUSTUM_PLANE].point = glm::vec3(0, 0, 0);
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov_y;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

    frustum_planes[BOTTOM_FRUSTUM_PLANE].point = glm::vec3(0, 0, 0);
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov_y;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

    frustum_planes[NEAR_FRUSTUM_PLANE].point = glm::vec3(0, 0, znear);
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

    frustum_planes[FAR_FRUSTUM_PLANE].point = glm::vec3(0, 0, zfar);
    frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

std::vector<Triangle> Polygon::clipped_triangles()
{
    clip();

    std::vector<Triangle> triangles;
    for (int i = 0; i < num_vertices - 2; i++) {
        Triangle triangle;
        int index0 = 0;
        int index1 = i + 1;
        int index2 = i + 2;

        triangle.points[0] = glm::vec4(vertices[index0], 1);
        triangle.points[1] = glm::vec4(vertices[index1], 1);
        triangle.points[2] = glm::vec4(vertices[index2], 1);

        triangle.uvs[0] = texcoords[index0];
        triangle.uvs[1] = texcoords[index1];
        triangle.uvs[2] = texcoords[index2];
        triangles.push_back(triangle);
    }
    return triangles;
}

void Polygon::clip()
{
    clip_against_plane(LEFT_FRUSTUM_PLANE);
    clip_against_plane(RIGHT_FRUSTUM_PLANE);
    clip_against_plane(TOP_FRUSTUM_PLANE);
    clip_against_plane(BOTTOM_FRUSTUM_PLANE);
    clip_against_plane(NEAR_FRUSTUM_PLANE);
    clip_against_plane(FAR_FRUSTUM_PLANE);
}

void Polygon::clip_against_plane(int plane)
{
    glm::vec3 plane_point = frustum_planes[plane].point;
    glm::vec3 plane_normal = frustum_planes[plane].normal;

    // Declare a static array of inside vertices that will be part of the final polygon returned via parameter
    glm::vec3 inside_vertices[MAX_NUM_POLY_VERTICES];
    glm::vec2 inside_texcoords[MAX_NUM_POLY_VERTICES];
    int num_inside_vertices = 0;

    // Start the current vertex with the first polygon vertex and texture coordinate
    glm::vec3* current_vertex = &vertices[0];
    glm::vec2* current_texcoord = &texcoords[0];

    // Start the previous vertex with the last polygon vertex and texture coordinate
    glm::vec3* previous_vertex = &vertices[num_vertices - 1];
    glm::vec2* previous_texcoord = &texcoords[num_vertices - 1];

    // Calculate the dot product of the current and previous vertex
    float current_dot = 0;
    float previous_dot = glm::dot(*previous_vertex - plane_point, plane_normal);

    // Loop all the polygon vertices while the current is different than the last one
    while (current_vertex != &vertices[num_vertices]) {
        current_dot = glm::dot(*current_vertex - plane_point, plane_normal);

        // If we changed from inside to outside or from outside to inside
        if (current_dot * previous_dot < 0) {
            // Find the interpolation factor t
            float t = previous_dot / (previous_dot - current_dot);

            // Calculate the intersection point I = Q1 + t(Q2-Q1)
            glm::vec3 intersection_point(
                float_lerp(previous_vertex->x, current_vertex->x, t),
                float_lerp(previous_vertex->y, current_vertex->y, t),
                float_lerp(previous_vertex->z, current_vertex->z, t));

            // Use the lerp formula to get the interpolated U and V texture coordinates
            glm::vec2 interpolated_texcoord = {
                float_lerp(previous_texcoord->x, current_texcoord->x, t),
                float_lerp(previous_texcoord->y, current_texcoord->y, t)
            };

            // Insert the intersection point to the list of "inside vertices"
            inside_vertices[num_inside_vertices] = glm::vec3(intersection_point);
            inside_texcoords[num_inside_vertices] = glm::vec2(interpolated_texcoord);
            num_inside_vertices++;
        }

        // Current vertex is inside the plane
        if (current_dot > 0) {
            // Insert the current vertex to the list of "inside vertices"
            inside_vertices[num_inside_vertices] = glm::vec3(*current_vertex);
            inside_texcoords[num_inside_vertices] = glm::vec2(*current_texcoord);
            num_inside_vertices++;
        }

        // Move to the next vertex
        previous_dot = current_dot;
        previous_vertex = current_vertex;
        previous_texcoord = current_texcoord;
        current_vertex++;
        current_texcoord++;
    }

    // At the end, copy the list of inside vertices into the destination polygon (out parameter)
    for (int i = 0; i < num_inside_vertices; i++) {
        vertices[i] = glm::vec3(inside_vertices[i]);
        texcoords[i] = glm::vec2(inside_texcoords[i]);
    }
    num_vertices = num_inside_vertices;
}

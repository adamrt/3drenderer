#include "camera.h"

static camera_t camera;

void camera_init(vec3_t position, vec3_t direction)
{
    camera.position = position;
    camera.direction = direction;
    camera.velocity = vec3_new(0, 0, 0);
    camera.yaw = 0.0;
    camera.pitch = 0.0;
}

vec3_t camera_get_position(void)
{
    return camera.position;
}

vec3_t camera_get_direction(void)
{
    return camera.direction;
}

vec3_t camera_get_velocity(void)
{
    return camera.velocity;
}

float camera_get_yaw(void)
{
    return camera.yaw;
}

float camera_get_pitch(void)
{
    return camera.pitch;
}

void camera_set_position(vec3_t position)
{
    camera.position = position;
}

void camera_set_direction(vec3_t direction)
{
    camera.direction = direction;
}

void camera_set_velocity(vec3_t velocity)
{
    camera.velocity = velocity;
}

void camera_rotate_yaw(float angle)
{
    camera.yaw += angle;
}

void camera_rotate_pitch(float angle)
{
    camera.pitch += angle;
}

vec3_t camera_get_lookat(void)
{
    // Initialize the target looking at the positive z-axis
    vec3_t target = { 0, 0, 1 };

    mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
    mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch);

    // Create camera rotation matrix based on yaw and pitch
    mat4_t camera_rotation = mat4_identity();
    camera_rotation = mat4_mul_mat4(camera_pitch_rotation, camera_rotation);
    camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_rotation);

    // Update camera direction based on the rotation
    vec4_t camera_direction = mat4_mul_vec4(camera_rotation, vec4_from_vec3(target));
    camera.direction = vec3_from_vec4(camera_direction);

    // Offset the camera position in the direction where the camera is pointing at
    target = vec3_add(camera.position, camera.direction);

    return target;
}

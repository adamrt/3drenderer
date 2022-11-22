#include "camera.h"
#include "matrix.h"

static camera_t camera;

void camera_init(glm::vec3 position, glm::vec3 direction)
{
    camera.position = position;
    camera.direction = direction;
    camera.velocity = glm::vec3(0, 0, 0);
    camera.yaw = 0.0;
    camera.pitch = 0.0;
}

glm::vec3 camera_get_position(void)
{
    return camera.position;
}

glm::vec3 camera_get_direction(void)
{
    return camera.direction;
}

glm::vec3 camera_get_velocity(void)
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

void camera_set_position(glm::vec3 position)
{
    camera.position = position;
}

void camera_set_direction(glm::vec3 direction)
{
    camera.direction = direction;
}

void camera_set_velocity(glm::vec3 velocity)
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

glm::vec3 camera_get_lookat(void)
{
    // Initialize the target looking at the positive z-axis
    glm::vec3 target = { 0, 0, 1 };

    mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
    mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch);

    // Create camera rotation matrix based on yaw and pitch
    mat4_t camera_rotation = mat4_identity();
    camera_rotation = mat4_mul_mat4(camera_pitch_rotation, camera_rotation);
    camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_rotation);

    // Update camera direction based on the rotation
    glm::vec4 camera_direction = mat4_mul_vec4(camera_rotation, glm::vec4(target, 1));
    camera.direction = camera_direction;

    // Offset the camera position in the direction where the camera is pointing at
    target = camera.position + camera.direction;

    return target;
}

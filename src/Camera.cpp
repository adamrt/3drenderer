#include "Camera.h"
#include "matrix.h"

Camera::Camera(glm::vec3 pos, glm::vec3 dir)
{
    position = pos;
    direction = dir;
}

glm::vec3 Camera::get_look_at()
{
    glm::vec3 target = { 0, 0, 1 };
    mat4_t camera_rotation = mat4_identity();
    direction = mat4_mul_vec4(camera_rotation, glm::vec4(target, 1));
    return position + direction;
}

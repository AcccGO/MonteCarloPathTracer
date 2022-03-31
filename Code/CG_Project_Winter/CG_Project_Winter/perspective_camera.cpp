#include "perspective_camera.h"

#include "math.h"

PerspectiveCamera::PerspectiveCamera()
{
    resetPerspectiveCamera(glm::dvec3{0, 0, 0}, glm::dvec3{0, 0, -1}, glm::dvec3{0, 1, 0}, 60, glm::dvec2{1280, 720});
}

PerspectiveCamera::PerspectiveCamera(glm::dvec3 pos, glm::dvec3 look_at, glm::dvec3 up, double fov, glm::dvec2 film_resolution)
{
    lookAt(pos, look_at, up);

    this->fov    = fov;
    this->width  = film_resolution.x;
    this->height = film_resolution.y;

    // Set the canvas size to pc_near plane.
    // GLM: unit of radians.
    this->pc_near = 0.5 * this->height / (glm::tan(this->fov * PI / 360.0));
    this->pc_far  = this->pc_near + 2000;  // The pc_far plane is arbitrary.
}

void PerspectiveCamera::resetPerspectiveCamera(glm::dvec3 pos, glm::dvec3 look_at, glm::dvec3 up, double fov, glm::dvec2 film_resolution)
{
    lookAt(pos, look_at, up);

    this->fov    = fov;
    this->width  = film_resolution.x;
    this->height = film_resolution.y;

    // Set the canvas size to pc_near plane.
    // GLM: unit of radians.
    this->pc_near = 0.5 * this->height / (glm::tan(this->fov * PI / 360.0));
    this->pc_far  = this->pc_near + 2000;
}

void PerspectiveCamera::lookAt(glm::dvec3 pos, glm::dvec3 look_at, glm::dvec3 up)
{
    this->position  = pos;
    this->direction = glm::normalize(look_at - pos);
    this->right     = glm::normalize(glm::cross(this->direction, up));
    this->up        = glm::normalize(glm::cross(this->right, this->direction));
}

// Transforms points on pixels to world coordinates.
// Pass in the coordinates between 0 and 1.
// Origin = center + (x - 0.5) * size * horizontal + (y - 0.5) * size * up.
Ray PerspectiveCamera::generateRay(glm::dvec2 point)
{
    glm::dvec3 view_direction = this->pc_near * this->direction + this->right * (point.x - 0.5f) * this->width + this->up * (point.y - 0.5f) * this->height;

    auto nvd = glm::normalize(view_direction);

    return Ray(position, nvd);
}
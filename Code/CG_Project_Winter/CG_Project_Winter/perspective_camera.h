#pragma once
#include <glm.hpp>
#include <vec3.hpp>
#include "ray.h"


class PerspectiveCamera
{
public:
	PerspectiveCamera();
	PerspectiveCamera(glm::dvec3 pos, glm::dvec3 look_at, glm::dvec3 up, double fov, glm::dvec2 film_resolution);

	void resetPerspectiveCamera(glm::dvec3 pos, glm::dvec3 look_at, glm::dvec3 up, double fov, glm::dvec2 film_resolution);
	void lookAt(glm::dvec3 pos, glm::dvec3 look_at, glm::dvec3 up);
	Ray generateRay(glm::dvec2 point);

	glm::dvec3 position;//camera center
	glm::dvec3 look_at;//look at
	glm::dvec3 direction;//sight direction
	glm::dvec3 up;
	glm::dvec3 right;

	double width, height;//film resolution
	double pc_near, pc_far;//pc_near/pc_far plane
	double fov;//filed of view
};
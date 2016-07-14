#pragma once

#include <glm/glm.hpp>

namespace fw
{

struct PointLight
{
	PointLight() : colour(1, 1, 1), intensity(1), attenuation(0.3, 0.1) {}
	PointLight(glm::vec3 colour, float intensity, glm::vec2 attenuation)
		: colour(colour), intensity(intensity), attenuation(attenuation) {}

	glm::vec3 colour;
	float intensity;
	glm::vec2 attenuation; // [0] = linear, [1] = quadratic
};

}
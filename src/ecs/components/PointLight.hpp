#pragma once

#include <glm/glm.hpp>

namespace fw
{

class PointLight
{
public:
	PointLight();
	PointLight(glm::vec3 colour, float intensity, glm::vec2 attenuation);

	float GetRadius() const;

	glm::vec3 colour;
	float intensity;
	glm::vec2 attenuation; // [0] = linear, [1] = quadratic
};

}
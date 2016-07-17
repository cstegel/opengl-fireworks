#pragma once

#include "Common.hpp"

#include <glm/glm.hpp>

namespace fw
{

class Physics
{
public:
	Physics()
		: velocity(0, 0, 0), acceleration(0, 0, 0), gravityMult(1.f)
	{}

	Physics(glm::vec3 velocity, glm::vec3 acceleration)
		: velocity(velocity), acceleration(acceleration), gravityMult(1.0f)
	{}

	Physics(glm::vec3 velocity, glm::vec3 acceleration, float gravityMult)
		: velocity(velocity), acceleration(acceleration), gravityMult(gravityMult)
	{}

	glm::vec3 velocity;
	glm::vec3 acceleration;
	float gravityMult;
};

}
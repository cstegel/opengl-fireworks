#pragma once

#include <glm/glm.hpp>

namespace fw
{

struct View
{
	View() = default;
	View(float fovy, float near, float far) : fovy(fovy), near(near), far(far) {}

	float fovy;
	float near;
	float far;
};

}
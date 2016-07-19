#include "ecs/components/PointLight.hpp"
#include <algorithm>

namespace fw
{


PointLight::PointLight() : colour(1, 1, 1), intensity(1), attenuation(0.3, 0.1)
{
}

PointLight::PointLight(glm::vec3 colour, float intensity, glm::vec2 attenuation)
	: colour(colour), intensity(intensity), attenuation(attenuation)
{
}

float PointLight::GetRadius() const
{
	return 2.0f - std::min(1.95f, 2.0f/(1.f + 0.05f*intensity));
}

}

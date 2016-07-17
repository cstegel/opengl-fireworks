#include "ecs/components/Transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Ecs.hh>

namespace fw
{

glm::mat4 Transform::GetModelTransform()
{
	glm::mat4 model;

	if (relativeTo != ecs::Entity())
	{
		if (!relativeTo.Has<Transform>())
		{
			throw std::runtime_error("cannot be relative to something that does not have a transform");
		}

		model = relativeTo.Get<Transform>()->GetModelTransform();
	}

	return model * this->translate * GetRotateMatrix() * this->scale;
}

glm::vec3 Transform::GetForwardVec(const glm::vec3 & worldForward)
{
	glm::mat4 normalMatWorldToModel((GetModelTransform()));
	return glm::normalize(glm::vec3(normalMatWorldToModel * glm::vec4(worldForward, 0)));
}

glm::vec3 Transform::GetPosition()
{
	return glm::vec3(GetModelTransform() * glm::vec4(0, 0, 0, 1));
}

void Transform::SetRelativeTo(ecs::Entity ent)
{
	if (!ent.Has<Transform>())
	{
		std::stringstream ss;
		ss << "Cannot set placement relative to " << ent
		   << " because it does not have a placement.";
		throw std::runtime_error(ss.str());
	}

	this->relativeTo = ent;
}

void Transform::Rotate(float radians, glm::vec3 axis)
{
	this->rotate = glm::rotate(this->rotate, radians, axis);
}

void Transform::Translate(glm::vec3 xyz)
{
	this->translate = glm::translate(this->translate, xyz);
}

void Transform::Translate(float x, float y, float z)
{
	Translate(glm::vec3(x, y, z));
}

void Transform::Scale(glm::vec3 xyz)
{
	this->scale = glm::scale(this->scale, xyz);
}

void Transform::Scale(float x, float y, float z)
{
	Scale(glm::vec3(x, y, z));
}

void Transform::Scale(float amount)
{
	Scale(glm::vec3(amount, amount, amount));
}

glm::mat4 Transform::GetRotateMatrix()
{
	return glm::mat4_cast(rotate);
}

void Transform::LookAt(glm::vec3 center, glm::vec3 worldUp)
{
	rotate = glm::quat_cast(
		glm::inverse(glm::lookAt(
			glm::vec3(translate*glm::vec4(0, 0, 0, 1)),
			center,
			worldUp
		))
	);
}

}
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Ecs.hh>

namespace fw
{

class Transform
{
public:
	Transform() {}
	Transform(glm::vec3 position);
	Transform(float x, float y, float z);

	void SetRelativeTo(ecs::Entity);

	/**
	 * Return the matrix for specifying the placement of the entity in the world.
	 * This involves computing the incremental model transforms for any entities
	 * that this placement is relative to.
	 */
	glm::mat4 GetModelTransform();

	/**
	 * Return the world-space direction that this transform causes the entity to face.
	 * This will be computed by obtaining the model transform so it may be slow
	 * for hierarchical entities.
	 */
	glm::vec3 GetForward(const glm::vec3 & worldForward);

	glm::vec3 GetPosition();

	/**
	 * Change the local rotation by "radians" amount about the local "axis"
	 */
	void Rotate(float radians, glm::vec3 axis);

	/**
	 * Change the local position by an amount in the local x, y, z planes
	 */
	void Translate(glm::vec3 xyz);
	void Translate(float x, float y, float z);

	/**
	 * Change the local scale by an amount in the local x, y, z planes.
	 * This will propagate to Transforms that are relative to this one.
	 */
	void Scale(glm::vec3 xyz);
	void Scale(float x, float y, float z);
	void Scale(float amount);

	/**
	 * Changes the rotation part of the transform so that the view is looking at "center"
	 */
	void LookAt(glm::vec3 center, glm::vec3 worldUp);

	glm::mat4 GetRotateMatrix();

	glm::mat4 translate;
	glm::mat4 scale;
	glm::quat rotate;

private:
	ecs::Entity relativeTo;
};

}
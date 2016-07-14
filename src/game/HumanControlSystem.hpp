#pragma once

#include "ecs/components/Controller.hpp"
#include "core/InputManager.hpp"

#include <glm/glm.hpp>
#include <Ecs.hh>

namespace fw
{
	class HumanControlSystem
	{
	public:
		HumanControlSystem(ecs::EntityManager &entities, InputManager &input);
		~HumanControlSystem();

		/**
		 * Call this once per frame
		 */
		bool Frame(double dtSinceLastFrame);

		/**
		 * Assigns a default HumanController to the given entity.
		 */
		ecs::Handle<HumanController> AssignController(ecs::Entity entity);

	private:

		/**
		 * Move an entity in the given local direction based on how much time has passed
		 * since last frame.
		 */
		void move(ecs::Entity entity, double dt, glm::vec3 normalizedDirection);

		static const float MOVE_SPEED;
		static const glm::vec2 CURSOR_SENSITIVITY;

		ecs::EntityManager & entities;
		InputManager & input;
	};
}
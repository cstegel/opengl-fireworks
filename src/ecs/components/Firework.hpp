#pragma once

#include "Common.hpp"

namespace fw
{

class Firework
{
public:
	enum class Type
	{
		RANDOM,
	};

	Firework() {}

	float explosionCountdown;
	uint numSplitsOnExplosion;
	uint numExplosionsLeft;
	Type type;
};

}
#pragma once

namespace fw
{

// values are distinct bits so they can be stored as a bitset
enum class RenderFeature
{
	NORMAL_MAPS = 1 << 0,
	SPECULAR_REFLECTIONS = 1 << 1,
	NUM_VALUES = 2,
};

}
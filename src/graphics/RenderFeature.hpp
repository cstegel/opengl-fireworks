#pragma once

namespace fw
{

// values are distinct bits so they can be stored as a bitset
enum class RenderFeature
{
	NORMAL_MAPS                       = 0,
	SPECULAR_REFLECTIONS              = 1,
	SCREEN_SPACE_VOLUMETRIC_LIGHTING  = 2,
	GAMMA_CORRECT                     = 3,
	HDR                               = 4,
	NUM_VALUES                        = 5,
};

}
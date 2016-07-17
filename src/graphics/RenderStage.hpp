#pragma once

#include "Common.hpp"

namespace fw
{

enum class RenderStage
{
	NOT_RENDERING = 0,
	GEOMETRY,
	SHADING,
	LIGHT_MODELS,
	VOLUMETRIC_LIGHTING,
	POST_PROCESS,

	NUM_STAGES
};

string to_string(RenderStage stage);

}
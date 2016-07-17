#include "graphics/RenderStage.hpp"

namespace fw
{

string to_string(RenderStage stage)
{
	if (stage == RenderStage::GEOMETRY) return "Geometry";
	if (stage == RenderStage::SHADING) return "Shading";
	if (stage == RenderStage::LIGHT_MODELS) return "Light models";
	if (stage == RenderStage::VOLUMETRIC_LIGHTING) return "Volumetric Lighting";
	if (stage == RenderStage::POST_PROCESS) return "Post Processing";

	throw std::invalid_argument("not a valid render stage");
}

}
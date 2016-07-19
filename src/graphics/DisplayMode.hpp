#pragma once

namespace fw
{

enum class DisplayMode
{
	REGULAR = 0,
	NORMALS = 1,
	ALBEDO = 2,
	SPECULAR = 3,
	SHININESS = 4,
	POSITION = 5,
	STENCIL = 6,
	BRIGHTNESS = 7,
	VOLUMETRIC_LIGHTING = 8,
	SHADOW_MAP_DEPTH = 9,
};

}
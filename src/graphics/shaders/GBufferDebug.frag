#version 440

layout (binding = 0) uniform sampler2D texPosition;
layout (binding = 1) uniform sampler2D texNormalShininess;
layout (binding = 2) uniform sampler2D texAlbedoSpecular;
layout (binding = 3) uniform usampler2D texStencil;

// these should match src/graphics/DisplayMode.hpp
const int DISPLAY_NORMALS = 1;
const int DISPLAY_ALBEDO = 2;
const int DISPLAY_SPECULAR = 3;
const int DISPLAY_SHININESS = 4;
const int DISPLAY_POSITION = 5;
const int DISPLAY_STENCIL = 6;

uniform int displayMode;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColour;

void main()
{
	if (displayMode == DISPLAY_NORMALS)
	{
		outFragColour = vec4((texture(texNormalShininess, inTexCoord).rgb + 1) / 2, 1.0f);
	}
	else if (displayMode == DISPLAY_ALBEDO)
	{
		outFragColour = vec4(texture(texAlbedoSpecular, inTexCoord).rgb, 1.0f);
	}
	else if (displayMode == DISPLAY_SPECULAR)
	{
		float spec = texture(texAlbedoSpecular, inTexCoord).a;
		outFragColour = vec4(spec, spec, spec, 1.0f);
	}
	else if (displayMode == DISPLAY_SHININESS)
	{
		// dull and tone map the shininess
		float shine = texture(texNormalShininess, inTexCoord).a / 10.0f;

		shine = shine / (1.0 + shine);
		outFragColour = vec4(shine, shine, shine, 1.0f);
	}
	else if (displayMode == DISPLAY_POSITION)
	{
		// divide to make visual transition smoother
		vec3 pos = texture(texPosition, inTexCoord).rgb / 2.0;

		outFragColour.r = 0.5 + (pos.x / abs(pos.x)) * 0.5 * abs(pos.x) / (1.0 + abs(pos.x));
		outFragColour.g = 0.5 + (pos.y / abs(pos.y)) * 0.5 * abs(pos.y) / (1.0 + abs(pos.y));
		outFragColour.b = 0.5 + (pos.z / abs(pos.z)) * 0.5 * abs(pos.z) / (1.0 + abs(pos.z));
	}
	else if (displayMode == DISPLAY_STENCIL)
	{
		uvec4 stencil = texture(texStencil, inTexCoord);
		outFragColour = vec4(float(stencil.r), float(stencil.g), float(stencil.a), 1.0f);
	}
	else
	{
		// indicate a bad display type (purple)
		outFragColour = vec4(1, 0, 1, 1);
	}
}
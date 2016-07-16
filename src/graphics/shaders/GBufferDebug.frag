#version 430

layout (binding = 0) uniform sampler2D texPosition;
layout (binding = 1) uniform sampler2D texNormal;
layout (binding = 2) uniform sampler2D texAlbedoSpecular;

// these should match src/graphics/DisplayMode.hpp
const int DISPLAY_NORMALS = 1;
const int DISPLAY_ALBEDO = 2;
const int DISPLAY_SPECULAR = 3;
const int DISPLAY_POSITION = 4;

uniform int displayMode;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColour;

void main()
{
	if (displayMode == DISPLAY_NORMALS)
	{
		outFragColour = vec4(texture(texNormal, inTexCoord).rgb, 1.0f);
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
	else if (displayMode == DISPLAY_POSITION)
	{
		vec3 pos = texture(texPosition, inTexCoord).rgb;// / 100f;
		// pos.x /= 100.0f;
		// pos.y /= 100.0f;
		// pos.z /= 100.0f;
		// pos.x = pos.x / (1.0 + pos.x);
		// pos.y = pos.y / (1.0 + pos.y);
		// pos.z = pos.z / (1.0 + pos.z);
		outFragColour = vec4(pos, 1.0f);
	}
	else
	{
		outFragColour = vec4(1, 0, 1, 1);
	}
}
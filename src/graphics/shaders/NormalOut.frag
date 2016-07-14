#version 430

uniform sampler2D texNormal;

uniform bool hasNormalTexture;

layout (location = 0) in vec3 inFragPos_World;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal_World;
layout (location = 3) in mat3 tangentToWorld;

layout (location = 0) out vec4 outFragColour;


void main()
{
	vec3 normal = inNormal_World;
	if (hasNormalTexture)
	{
		normal = tangentToWorld * normalize(texture(texNormal, inTexCoord).rgb * 2.0 - 1.0);
	}

	outFragColour = vec4((normal + 1.0) / 2.0, 1);
}
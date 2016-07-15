#version 430 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal_World;
layout (location = 3) in mat3 tangentToWorld;

// these go to the gbuffer textures
layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec4 outAlbedoSpec;

uniform bool hasNormalTexture;
uniform sampler2D texAlbedo;
uniform sampler2D texSpecular;
uniform sampler2D texNormal;

void main()
{
	outPosition = inPosition;
	outAlbedoSpec.rgb = texture(texAlbedo, inTexCoord).rgb;
	outAlbedoSpec.a = texture(texSpecular, inTexCoord).r;

	if (hasNormalTexture)
	{
		outNormal = texture(texNormal, inTexCoord).xyz;
		outNormal = normalize(tangentToWorld * outNormal);
	}
	else
	{
		outNormal = (inNormal_World + 1) / 2.0f;
	}
}
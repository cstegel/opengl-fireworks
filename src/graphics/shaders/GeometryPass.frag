#version 430 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal_World;
layout (location = 3) in mat3 tangentToWorld;

// these go to the gbuffer textures
layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec4 outNormalShininess;
layout (location = 2) out vec4 outAlbedoSpec;
layout (location = 3) out uvec4 outStencil;

uniform bool hasNormalTexture;
uniform float shininess;
uniform sampler2D texAlbedo;
uniform sampler2D texSpecular;
uniform sampler2D texNormal;

void main()
{
	outPosition = inPosition;
	outAlbedoSpec.rgb = texture(texAlbedo, inTexCoord).rgb;
	outAlbedoSpec.a = texture(texSpecular, inTexCoord).r;

	vec3 normal_World;
	if (hasNormalTexture)
	{
		normal_World = texture(texNormal, inTexCoord).xyz * 2.0 - 1.0;
		normal_World = normalize(tangentToWorld * normal_World);
	}
	else
	{
		normal_World = normalize(inNormal_World);
	}

	outNormalShininess.rgb = normal_World;
	outNormalShininess.a = shininess;

	outStencil.r = 1; // rest of vector is currently unused
}
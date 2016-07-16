#version 430

struct PointLight
{
	vec3 position;
	vec3 colour;

	float intensity;
	vec2 attenuation; // [0] = linear, [1] = quadratic
};

#define N_POINT_LIGHTS 16
uniform PointLight[N_POINT_LIGHTS] pointLights;

layout (binding = 0) uniform sampler2D texPosition;
layout (binding = 1) uniform sampler2D texNormal;
layout (binding = 2) uniform sampler2D texAlbedoSpecular;

uniform float shininess;
uniform vec3 viewPos_World;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColour;

void main()
{
	float brightness = 0.0f;

	for (int i = 0; i < N_POINT_LIGHTS; i++)
	{
		PointLight light = pointLights[i];
		vec3 fragPos_World = texture(texPosition, inTexCoord).xyz;
		vec3 normal_World = texture(texNormal, inTexCoord).xyz;

		vec3 toLight = light.position - fragPos_World;

		float dist = length(toLight);
		float localBrightness =
		light.intensity /
			(1 + light.attenuation[0]*dist + pow(light.attenuation[1], 2)*dist);

		brightness += localBrightness;
	}

	// simple reinhardt tone mapping
	brightness = brightness / (1.0 + brightness);
	outFragColour = vec4(brightness, brightness, brightness, 1.0f);

	// prevent viewPos_World from being optimized out so we can still get errors in real shader
	outFragColour.r += min(0, max(0, viewPos_World.x));
}


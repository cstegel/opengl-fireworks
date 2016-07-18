#version 430

struct PointLight
{
	vec2 position_Screen;
	vec3 colour;

	bool behindViewer;
	float intensity;
	float viewAngleReduction;
	// vec2 attenuation; // [0] = linear, [1] = quadratic
};

#define N_POINT_LIGHTS 16
const uint N_SAMPLES = 200;
const float density = 1.0f;
const float decay = 0.8f;
const float exposure = 1.0f;
const float weight = 1.0f;

layout (binding = 2) uniform sampler2D texAlbedoSpecular;
layout (binding = 3) uniform usampler2D texStencil;
uniform PointLight[N_POINT_LIGHTS] pointLights;
uniform int numLights;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColour;

void main()
{
	vec3 accumulatedColour = vec3(0, 0, 0);

	for (int i = 0; i < min(numLights, N_POINT_LIGHTS); i++)
	{
		PointLight light = pointLights[i];
		if (light.behindViewer)
		{
			continue;
		}

		vec3 lightShaftColour = vec3(0, 0, 0);

		vec2 texCoord = inTexCoord;
		float illumDecay = 1.0f;

		vec2 dTexCoord = light.position_Screen - inTexCoord;
		dTexCoord /= N_SAMPLES * density;

		for (int j = 0; j < N_SAMPLES; j++)
		{
			// stencil is stored where 1 == occluder and 0 == free so we invert this
			uint stencilBit = texture(texStencil, texCoord).r;
			uint invStencilBit = 1 ^ stencilBit; // only flip the first bit
			vec3 colour = invStencilBit * texture(texAlbedoSpecular, texCoord).rgb;

			lightShaftColour += weight * illumDecay * colour;

			illumDecay *= decay;
			texCoord += dTexCoord;
		}

		accumulatedColour += lightShaftColour * light.viewAngleReduction;
	}

	outFragColour = vec4(exposure * accumulatedColour, 1.0f);
}
#version 430

struct PointLight
{
	vec2 position_Screen;
	vec3 colour;

	float intensity;
	// vec2 attenuation; // [0] = linear, [1] = quadratic
};

#define N_POINT_LIGHTS 1
const uint N_SAMPLES = 100;
const float density = 1.0f;
const float decay = 0.8f;
const float exposure = 1.0f;
const float weight = 1.0f;

layout (binding = 2) uniform sampler2D texAlbedoSpecular;
layout (binding = 3) uniform usampler2D texStencil;
uniform PointLight[N_POINT_LIGHTS] pointLights;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColour;

void main()
{
	vec3 accumulatedColour = vec3(0, 0, 0);

	for (int i = 0; i < N_POINT_LIGHTS; i++)
	{
		vec3 lightShaftColour = vec3(0, 0, 0);

		vec2 texCoord = inTexCoord;
		float illumDecay = 1.0f;
		PointLight light = pointLights[i];

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

		accumulatedColour += lightShaftColour;
	}

	outFragColour = vec4(exposure * accumulatedColour, 1.0f);
}
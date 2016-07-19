#version 430

struct PointLight
{
	vec3 position;
	vec3 colour;

	float intensity;
	vec2 attenuation; // [0] = linear, [1] = quadratic
	sampler2D texShadowDepth;
	mat4 worldToLightMat;
};

#define N_POINT_LIGHTS 8
uniform PointLight[N_POINT_LIGHTS] pointLights;

layout (binding = 0) uniform sampler2D texPosition;
layout (binding = 1) uniform sampler2D texNormalShininess;
layout (binding = 2) uniform sampler2D texAlbedoSpecular;

uniform int numLights;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColour;


void main() {
	float depth = texture(pointLights[0].texShadowDepth, inTexCoord).r;
	outFragColour = vec4(depth, depth, depth, 1.0);

	// prevent things being optimized out and requiring conditional binding in the source code
	outFragColour.r += min(0, max(0, pointLights[0].position.x));
	outFragColour.r += min(0, max(0, pointLights[0].colour.x));
	outFragColour.r += min(0, max(0, pointLights[0].intensity));
	outFragColour.r += min(0, max(0, pointLights[0].attenuation.x));
	outFragColour.r += min(0, max(0, pointLights[0].worldToLightMat[0][0]));
	outFragColour.r += min(0, max(0, numLights));
}

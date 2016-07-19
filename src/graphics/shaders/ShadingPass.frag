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

uniform vec3 viewPos_World;
uniform int numLights;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColour;

float getShadow(vec3 fragPos_World, PointLight light)
{
	vec4 fragPos_Light = light.worldToLightMat * vec4(fragPos_World, 1);

	// homogenize
	fragPos_Light.xyz /= fragPos_Light.w;

	// convert to [0, 1] range
	vec3 fragPos_Screen = fragPos_Light.xyz * 0.5 + 0.5;

	float occluderDepth = texture(light.texShadowDepth, fragPos_Screen.xy).r;
	float curDepth = fragPos_Screen.z;

	return curDepth < occluderDepth ? 0.0 : 1.0;
}

vec3 phongColour(PointLight light)
{
	vec3 fragPos_World = texture(texPosition, inTexCoord).xyz;
	vec3 normal_World = texture(texNormalShininess, inTexCoord).xyz;

	vec3 toLight = light.position - fragPos_World;

	float dist = length(toLight);
	float localBrightness =
		light.intensity /
		(1 + light.attenuation[0]*dist + pow(light.attenuation[1], 2)*dist);

	vec3 matDiffuse = texture(texAlbedoSpecular, inTexCoord).rgb;
	vec3 ambient = 0.05 * light.colour * matDiffuse;

	float diffuseStrength = max(0, dot(normalize(toLight), normal_World));
	vec3 diffuse = diffuseStrength * matDiffuse;

	vec3 reflectDir = normalize(reflect(-toLight, normal_World));
	vec3 toViewer = normalize(viewPos_World - fragPos_World);
	float specularStrength = max(0, dot(reflectDir, toViewer));

	float specCoeff = texture(texAlbedoSpecular, inTexCoord).a;
	vec3 matSpec = vec3(specCoeff, specCoeff, specCoeff);

	float shininess = texture(texNormalShininess, inTexCoord).a;
	vec3 specular = pow(specularStrength, shininess) * matSpec;

	float shadow = getShadow(fragPos_World, light);
	return ambient + (1.0 - shadow) * localBrightness * light.colour *  (diffuse + specular);
}

void main() {
	vec3 colour = vec3(0, 0, 0);

	if (texture(texPosition, inTexCoord).xyz == vec3(0, 0, 0))
	{
		discard;
	}

	for (int i = 0; i < min(numLights, N_POINT_LIGHTS); ++i)
	{
		colour += phongColour(pointLights[i]);
	}

	outFragColour = vec4(colour, 1.0);
}

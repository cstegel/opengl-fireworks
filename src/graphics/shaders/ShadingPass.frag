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


vec3 phongColour(PointLight light)
{
	vec3 fragPos_World = texture(texPosition, inTexCoord).xyz;
	vec3 normal_World = texture(texNormal, inTexCoord).xyz;

	vec3 toLight = light.position - fragPos_World;

	float dist = length(toLight);
	float localBrightness =
		light.intensity /
		(1 + light.attenuation[0]*dist + pow(light.attenuation[1], 2)*dist);

	vec3 matDiffuse = texture(texAlbedoSpecular, inTexCoord).rgb;
	vec3 ambient = 0.05 * light.colour * matDiffuse;

	float diffuseStrength = max(0, dot(normalize(toLight), normal_World));
	vec3 diffuse = diffuseStrength * matDiffuse;

	float specCoeff = texture(texAlbedoSpecular, inTexCoord).a;
	vec3 matSpec = vec3(specCoeff, specCoeff, specCoeff);
	vec3 reflectDir = normalize(reflect(toLight, normal_World));
	vec3 toViewer = normalize(viewPos_World - fragPos_World);
	float specularStrength = max(0, dot(reflectDir, toViewer));

	vec3 specular = pow(specularStrength, shininess) * matSpec;

	return ambient + localBrightness * light.colour * (diffuse + specular);
}

vec3 gammaCorrect(vec3 colour)
{
	return pow(colour, vec3(1/2.2, 1/2.2, 1/2.2));
}

// as explained by http://filmicgames.com/archives/75
vec3 uncharted2Tonemap(vec3 x)
{
	const float A = 0.15;
	const float B = 0.50;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.02;
	const float F = 0.30;
	const float W = 11.2;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main() {
	vec3 colour = vec3(0, 0, 0);

	for (int i = 0; i < N_POINT_LIGHTS; ++i)
	{
		colour += phongColour(pointLights[i]);
	}

	colour = uncharted2Tonemap(colour);
	colour = gammaCorrect(colour);

	outFragColour = vec4(colour, 1.0);
}

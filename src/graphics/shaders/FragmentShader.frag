#version 430

struct PointLight
{
	vec3 position;
	vec3 colour;

	float intensity;
	vec2 attenuation; // [0] = linear, [1] = quadratic
};

#define N_POINT_LIGHTS 10
uniform PointLight[N_POINT_LIGHTS] pointLights;




uniform sampler2D texDiffuse;
uniform sampler2D texSpecular;
uniform sampler2D texNormal;
uniform float shininess;
uniform vec3 viewPos_World;

uniform bool hasDiffuseTexture;
uniform bool hasSpecularTexture;
uniform bool hasNormalTexture;

layout (location = 0) in vec3 inFragPos_World;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal_World;
layout (location = 3) in mat3 tangentToWorld;

layout (location = 0) out vec4 outFragColour;


vec3 phongColour(PointLight light)
{
	vec3 toLight = light.position - inFragPos_World;

	float dist = length(toLight);
	float localBrightness =
		light.intensity /
		(1 + light.attenuation[0]*dist + pow(light.attenuation[1], 2)*dist);


	vec3 matDiffuse = vec3(1, 1, 1);
	if (hasDiffuseTexture)
	{
		matDiffuse = texture(texDiffuse, inTexCoord).rgb;
	}

	vec3 matSpec = vec3(1, 1, 1);
	if (hasSpecularTexture)
	{
		matSpec = texture(texSpecular, inTexCoord).rgb;
	}

	vec3 normal_World;
	if (hasNormalTexture)
	{
		vec3 normal_Tangent = texture(texNormal, inTexCoord).xyz * 2.0 - 1.0;
		normal_World = normalize(tangentToWorld * normal_Tangent);
	}
	else
	{
		normal_World = inNormal_World;
	}

	vec3 ambient = 0.05 * light.colour * matDiffuse;

	float diffuseStrength = max(0, dot(normalize(toLight), normal_World));
	vec3 diffuse = diffuseStrength * matDiffuse;

	vec3 reflectDir = normalize(reflect(toLight, normal_World));
	vec3 toViewer = normalize(viewPos_World - inFragPos_World);
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

	// colour.r = max(0, min(0, colour.r));
	// colour.g = max(0, min(0, colour.g));
	// colour.b = max(0, min(0, colour.b));
	//
	// colour += texture(texDiffuse, inTexCoord).rgb;

	outFragColour = vec4(colour, 1.0);
}

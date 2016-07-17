#version 430

layout (binding = 0) uniform sampler2D texPostProcessInput;
uniform bool doHDR;
uniform bool doGammaCorrect;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColour;

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

void main()
{
	vec3 colour = texture(texPostProcessInput, inTexCoord).rgb;

	if (doHDR)
	{
		colour = uncharted2Tonemap(colour);
	}

	if (doGammaCorrect)
	{
		colour = gammaCorrect(colour);
	}

	outFragColour = vec4(colour, 1.0);
}
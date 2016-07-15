#version 430

uniform sampler2D texNormal;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColour;

void main()
{
	outFragColour = vec4(texture(texNormal, inTexCoord).rgb, 1.0f);
	// outFragColour.rgb += 0.6;
}
#version 430

uniform vec3 lightColour;

layout (location = 0) out vec4 outFragColour;


void main()
{
	outFragColour = vec4(lightColour, 1);
}
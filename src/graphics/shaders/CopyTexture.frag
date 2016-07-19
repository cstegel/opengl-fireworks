#version 430

layout (binding = 0) uniform sampler2D texToCopy;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColour;

void main()
{
	outFragColour = texture(texToCopy, inTexCoord);
}
#version 430 core

layout (location = 0) in vec3 inPosition;

uniform mat4 worldToLightMat;
uniform mat4 modelMat;

void main()
{
	gl_Position = worldToLightMat * modelMat * vec4(inPosition, 1);
}
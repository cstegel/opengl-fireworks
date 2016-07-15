#version 430 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;
layout (location = 4) in vec2 inTexCoord;

layout (location = 0) out vec3 outPosition;
layout (location = 1) out vec2 outTexCoord;
layout (location = 2) out vec3 outNormal_World;
layout (location = 3) out mat3 tangentToWorld;

uniform mat4 mvp;
uniform mat3 normalMat;    // convert vectors from model -> world

void main()
{
	outPosition = vec3(mvp * vec4(inPosition, 1));

	// convert basis vectors to be with respect to world
	vec3 T_world = normalize(normalMat * inTangent);
	vec3 N_world = normalize(normalMat * inNormal);

	// Gram-Schmidt to ensure basis vectors are orthogonal (bitangent not actually used)
	T_world = normalize(T_world - dot(T_world, N_world) * N_world);
	vec3 B_world = cross(T_world, N_world);

	// change of basis from tangent space to world space
	tangentToWorld = mat3(T_world, B_world, N_world);

	outTexCoord = inTexCoord;
	outNormal_World = N_world;
}
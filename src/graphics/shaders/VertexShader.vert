#version 430


uniform mat4 project;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMat;    // convert vectors from model -> world

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;
layout (location = 4) in vec2 inTexCoord;

layout (location = 0) out vec3 outFragPos_World;
layout (location = 1) out vec2 outTexCoord;
layout (location = 2) out vec3 outNormal_World;
layout (location = 3) out mat3 tangentToWorld;

void main() {
	vec4 vertPos_World = model * vec4(inPosition, 1);
	gl_Position = project * view * vertPos_World;

	// convert basis vectors to be with respect to world
	vec3 T_world = normalize(normalMat * inTangent);
	vec3 N_world = normalize(normalMat * inNormal);

	T_world = normalize(T_world - dot(T_world, N_world) * N_world);
	vec3 B_world = cross(T_world, N_world);

	// vec3 B_world = normalize(normalMat * inBitangent);

	// change of basis from tangent space to world space
	tangentToWorld = mat3(T_world, B_world, N_world);

	outFragPos_World = vec3(vertPos_World);
	outTexCoord = inTexCoord;
	outNormal_World = N_world;
}

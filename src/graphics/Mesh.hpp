#pragma once

#include "Common.hpp"
#include "cs488/ShaderProgram.hpp"
#include "graphics/Texture.hpp"

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace fw
{

class RenderContext;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	glm::vec2 texCoords;
};

/**
 * vertex attrib layout in shaders are assumed to be position = 0, normal = 1, texcoord = 2
 */
class Mesh {
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	float shininess; // for when there is no shininess map

	Mesh(
		vector<Vertex> && vertices,
		vector<GLuint> && indices,
		vector<Texture> && textures,
		float shininess=0.0f);

	/**
	 * Assumes that the given shader is already bound
	 */
	void Render(
		const RenderContext & context,
		ShaderProgram & shader,
		const glm::mat4 & transform);

private:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	void setupMesh();
};

}
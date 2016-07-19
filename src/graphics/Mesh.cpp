#include "Mesh.hpp"
#include "graphics/RenderContext.hpp"
#include "graphics/RenderStage.hpp"
#include <glm/gtc/type_ptr.hpp>


namespace fw
{

Mesh::Mesh(
	vector<Vertex> && vertices,
	vector<GLuint> && indices,
	vector<Texture> && textures,
	float shininess)
	: vertices(vertices), indices(indices), textures(textures), shininess(shininess)
{
	setupMesh();
}

void Mesh::Render(
	const RenderContext & context,
	ShaderProgram & shader,
	const glm::mat4 & transform)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glm::mat3 transposeNormalMatrix = glm::inverse(glm::mat3(transform));

	glm::mat4 mvp = context.GetCachedProjection() * context.GetCachedView() * transform;

	glUniformMatrix4fv(
		shader.getUniformLocation("mvpMat", true),
		1, GL_FALSE, glm::value_ptr(mvp)
	);

	if (context.GetRenderStage() != RenderStage::LIGHT_MODELS)
	{
		// transpose it as it is loaded
		glUniformMatrix3fv(
		shader.getUniformLocation("normalMat", true),
		1, GL_TRUE, glm::value_ptr(transposeNormalMatrix)
		);

		glUniformMatrix4fv(
		shader.getUniformLocation("modelMat"),
		1, GL_FALSE, glm::value_ptr(transform)
		);

		glUniform1f(
		shader.getUniformLocation("shininess", true),
		shininess
		);
	}

	bool hasNormalTexture = false;

	for (uint textureNumber = 0; textureNumber < textures.size(); textureNumber++)
	{
		Texture & texture = textures.at(textureNumber);
		if (texture.type == Texture::Type::NORMAL)
		{
			if (!context.IsRenderFeatureEnabled(RenderFeature::NORMAL_MAPS))
			{
				continue;
			}
			hasNormalTexture = true;
		}
		texture.Bind(shader, textureNumber);
	}

	glUniform1i(shader.getUniformLocation("hasNormalTexture", true), hasNormalTexture);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	for (uint textureNumber = 0; textureNumber < textures.size(); textureNumber++)
	{
		Texture & texture = textures.at(textureNumber);
		texture.Unbind(textureNumber);
	}

	glBindVertexArray(0);
}


void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex),
		vertices.data(), GL_STATIC_DRAW
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(GLuint),
		indices.data(), GL_STATIC_DRAW
	);

	uint location = 0;

	// Vertex Positions
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(
		location,                            // location
		3,                                   // # components
		GL_FLOAT,                            // type
		GL_FALSE,                            // normalize when accessed?
		sizeof(Vertex),                      // stride
		(GLvoid*)offsetof(Vertex, position)  // offset
	);
	location++;

	// Vertex Normals
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(
		location, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, normal)
	);
	location++;

	// Vertex Tangents
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(
		location, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, tangent)
	);
	location++;

	// Vertex Bitangents
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(
		location, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, bitangent)
	);
	location++;

	// Vertex Texture Coords
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(
		location, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, texCoords)
	);
	location++;

	glBindVertexArray(0);
}

}
#pragma once

#include "Common.hpp"
#include "cs488/ShaderProgram.hpp"

#include <GL/glew.h>
#include <assimp/scene.h>
#include <iostream>

namespace fw
{

class Texture {
public:
	enum class Type
	{
		NORMAL,
		POSITION,
		ALBEDO,
		SPECULAR,
		ALBEDO_SPECULAR,
		SHININESS,
		STENCIL,
	};

	Texture() {}
	Texture(
		Type texType,
		uint width,
		uint height,
		GLint internalFormat,
		GLenum format,
		GLenum type,
		const GLvoid * image);

	~Texture();

	/**
	* Returns the id of the loaded texture.
	*/
	static GLuint LoadFromFile(const string & path);

	static Type GetType(aiTextureType aiType);

	void Bind(ShaderProgram & shader, uint textureNumber) const;
	void Unbind(uint textureNumber) const;

	GLuint id;
	Type type;
	string path;

	static unordered_map<aiTextureType, Type, EnumHash> aiTypeToTexType;
	static unordered_map<Type, string, EnumHash> typeToGlslVar;

	friend std::ostream & operator<<(std::ostream & out, const Texture & tex);
private:

	/**
	 * Returns the name of the sampler2d variable that this texture
	 * should be bound to.
	 */
	string getShaderVar() const;
};



}
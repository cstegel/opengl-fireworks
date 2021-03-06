#include "graphics/Texture.hpp"

#include <SOIL.h>
#include <iostream>

namespace fw
{

unordered_map<Texture::Type, string, EnumHash> Texture::typeToGlslVar = {
	{Texture::Type::ALBEDO, "texAlbedo"},
	{Texture::Type::SPECULAR, "texSpecular"},
	{Texture::Type::SHININESS, "texShininess"},
	{Texture::Type::NORMAL, "texNormal"},
	{Texture::Type::POSITION, "texPosition"},
	{Texture::Type::ALBEDO_SPECULAR, "texAlbedoSpecular"},
	{Texture::Type::STENCIL, "texStencil"},
	{Texture::Type::POST_PROCESS_INPUT, "texPostProcessInput"},
};

unordered_map<aiTextureType, Texture::Type, EnumHash> Texture::aiTypeToTexType = {
	{aiTextureType_DIFFUSE, Texture::Type::ALBEDO},
	{aiTextureType_SPECULAR, Texture::Type::SPECULAR},
	{aiTextureType_SHININESS, Texture::Type::SHININESS},
	{aiTextureType_HEIGHT, Texture::Type::NORMAL},
};

Texture::Texture(
	Type texType,
	uint width,
	uint height,
	GLint internalFormat,
	GLenum format,
	GLenum type,
	const GLvoid * image)
	: type(texType)
{
	glGenTextures(1, &id);

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (image)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
}

// with help from http://www.learnopengl.com/#!Model-Loading/Model
GLuint Texture::LoadFromFile(const string & path)
{
	// Generate texture ID and load texture data
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width;
	int height;
	int channels;

	unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
	string resultMsg(SOIL_last_result());

	if (resultMsg.compare("Image loaded") != 0)
	{
		throw std::runtime_error("error loading image \"" + path + "\": " + resultMsg);
	}

	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return textureID;
}

string Texture::getShaderVar() const
{
	return Texture::typeToGlslVar.at(type);
}

Texture::Type Texture::GetType(aiTextureType aiType)
{
	return aiTypeToTexType.at(aiType);
}

void Texture::Bind(ShaderProgram & shader, uint textureNumber) const
{
	glActiveTexture(GL_TEXTURE0 + textureNumber);
	glBindTexture(GL_TEXTURE_2D, this->id);
	glUniform1i(
		shader.getUniformLocation(getShaderVar(), true),
		textureNumber
	);
}

void Texture::Unbind(uint textureNumber) const
{
	glActiveTexture(GL_TEXTURE0 + textureNumber);
	glBindTexture(GL_TEXTURE_2D, 0);
}

std::ostream & operator<<(std::ostream & out, const Texture & tex)
{
	out << tex.getShaderVar() << ": " << tex.path;
	return out;
}

}
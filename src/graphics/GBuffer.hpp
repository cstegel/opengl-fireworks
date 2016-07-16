#pragma once

#include "Common.hpp"

#include "graphics/Texture.hpp"
#include "cs488/ShaderProgram.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace fw
{

class GBuffer
{
public:
	GBuffer(uint screenWidth, uint screenHeight);
	~GBuffer();

	void BindCoreTextures(ShaderProgram & shader);
	void BindStencilTexture(ShaderProgram & shader);
	void UnbindTextures();

	GLuint FBO; // frame buffer object

private:
	Texture texPosition;
	Texture texNormalShininess;
	Texture texAlbedoSpec;
	GLuint depthBuff;
	Texture texStencil;

	uint screenWidth;
	uint screenHeight;
};
}
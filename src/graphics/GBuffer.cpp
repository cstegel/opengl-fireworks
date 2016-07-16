#include "graphics/GBuffer.hpp"

namespace fw
{

GBuffer::GBuffer(uint screenWidth, uint screenHeight)
	: screenWidth(screenWidth), screenHeight(screenHeight)
{
	GLint maxbuffers;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxbuffers);
	std::cout << "Max Color Attachments: " << maxbuffers << std::endl;

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	texPosition = Texture(
		Texture::Type::POSITION,
		screenWidth,
		screenHeight,
		GL_RGB32F ,
		GL_RGB,
		GL_FLOAT,
		NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texPosition.id, 0);

	texNormalShininess = Texture(
		Texture::Type::NORMAL,
		screenWidth,
		screenHeight,
		GL_RGBA16F,
		GL_RGBA,
		GL_FLOAT,
		NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texNormalShininess.id, 0);

	texAlbedoSpec = Texture(
		Texture::Type::ALBEDO_SPECULAR,
		screenWidth,
		screenHeight,
		GL_RGBA16F,
		GL_RGBA,
		GL_FLOAT,
		NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texAlbedoSpec.id, 0);

	// Use these colour attachments for rendering
	GLuint attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	// create the depth buffer
	glGenRenderbuffers(1, &depthBuff);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuff);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuff);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("G-Buffer is not complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GBuffer::~GBuffer()
{
}

void GBuffer::BindTextures(ShaderProgram & shader)
{
	texPosition.Bind(shader, 0);
	texNormalShininess.Bind(shader, 1);
	texAlbedoSpec.Bind(shader, 2);
}

void GBuffer::UnbindTextures()
{
	texPosition.Unbind(0);
	texNormalShininess.Unbind(1);
	texAlbedoSpec.Unbind(2);
}

}
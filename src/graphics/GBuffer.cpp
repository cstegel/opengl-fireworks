#include "graphics/GBuffer.hpp"
#include "cs488/GlErrorCheck.hpp"

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

	texAlbedoSpec = Texture(
		Texture::Type::ALBEDO_SPECULAR,
		screenWidth,
		screenHeight,
		GL_RGBA16F,
		GL_RGBA,
		GL_FLOAT,
		NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texAlbedoSpec.id, 0);

	// I can't for the life of me get this working as a stencil attachment so I'm using colour attachment
	texStencil = Texture(
		Texture::Type::STENCIL,
		screenWidth,
		screenHeight,
		GL_RGB32UI,
		GL_RGB_INTEGER,
		GL_UNSIGNED_INT,
		NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, texStencil.id, 0);

	// Use these colour attachments for rendering
	GLuint attachments[4] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};
	glDrawBuffers(4, attachments);

	// create the depth buffer
	// glGenRenderbuffers(1, &depthBuff);
	// glBindRenderbuffer(GL_RENDERBUFFER, depthBuff);
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuff);

	// glGenRenderbuffersEXT(1, &depth_stencil_rb);
	// glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_stencil_rb);
	// glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH24_STENCIL8_EXT, 256, 256);
	// glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_stencil_rb);
	// glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_stencil_rb);



	glGenTextures(1, &depthBuff);
	glBindTexture(GL_TEXTURE_2D, depthBuff);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH_COMPONENT,
		screenWidth,
		screenHeight,
		0,
		GL_DEPTH_COMPONENT,
		GL_FLOAT,
		NULL);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuff, 0);

	CHECK_GL_ERRORS;

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

void GBuffer::BindCoreTextures(ShaderProgram & shader)
{
	texPosition.Bind(shader, 0);
	texNormalShininess.Bind(shader, 1);
	texAlbedoSpec.Bind(shader, 2);
}

void GBuffer::BindStencilTexture(ShaderProgram & shader)
{
	texStencil.Bind(shader, 3);
}

void GBuffer::UnbindTextures()
{
	texPosition.Unbind(0);
	texNormalShininess.Unbind(1);
	texAlbedoSpec.Unbind(2);
	texStencil.Unbind(3);
}

}
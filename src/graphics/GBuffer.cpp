#include "graphics/GBuffer.hpp"

namespace fw
{

GBuffer::GBuffer(uint screenWidth, uint screenHeight)
	: screenWidth(screenWidth), screenHeight(screenHeight)
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// - Position color buffer
    glGenTextures(1, &texPosition);
    glBindTexture(GL_TEXTURE_2D, texPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texPosition, 0);

    // - Normal color buffer
    glGenTextures(1, &texNormal);
    glBindTexture(GL_TEXTURE_2D, texNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texNormal, 0);

    // - Color + Specular color buffer
    glGenTextures(1, &texAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, texAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texAlbedoSpec, 0);

    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // - Create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &depthBuff);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuff);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuff);


	//
	// texPosition = Texture(
	// 	Texture::Type::POSITION,
	// 	screenWidth,
	// 	screenHeight,
	// 	GL_RGBA16F ,
	// 	GL_RGB,
	// 	GL_FLOAT,
	// 	NULL);

	// // position texture
	// glGenTextures(1, &texPosition);
	// glBindTexture(GL_TEXTURE_2D, texPosition);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texPosition.id, 0);
	//
	// texNormal = Texture(
	// 	Texture::Type::NORMAL,
	// 	screenWidth,
	// 	screenHeight,
	// 	GL_RGBA16F ,
	// 	GL_RGB,
	// 	GL_FLOAT,
	// 	NULL);
	//
	// // normal texture
	// // glGenTextures(1, &normalTex);
	// // glBindTexture(GL_TEXTURE_2D, normalTex);
	// // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	// // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texNormal.id, 0);
	//
	// texAlbedoSpec = Texture(
	// 	Texture::Type::ALBEDO_SPECULAR,
	// 	screenWidth,
	// 	screenHeight,
	// 	GL_RGBA,
	// 	GL_RGBA,
	// 	GL_UNSIGNED_BYTE,
	// 	NULL);
	//
	// // diffuse + specular texture
	// // glGenTextures(1, &albedoSpecTex);
	// // glBindTexture(GL_TEXTURE_2D, albedoSpecTex);
	// // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	// // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texAlbedoSpec.id, 0);
	//
	// // Use these colour attachments for rendering
	// GLuint attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	// glDrawBuffers(3, attachments);
	//
	// // create the depth buffer
	// glGenRenderbuffers(1, &depthBuff);
	// glBindRenderbuffer(GL_RENDERBUFFER, depthBuff);
	// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuff);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("G-Buffer is not complete");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GBuffer::~GBuffer()
{
}

void GBuffer::BindTextures(ShaderProgram & shader)
{
	// texPosition.Bind(shader, 0);
	// texNormal.Bind(shader, 1);
	// texAlbedoSpec.Bind(shader, 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texPosition);
	glUniform1i(
		shader.getUniformLocation("texPosition", true),
		GL_TEXTURE0
	);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texNormal);
	glUniform1i(
		shader.getUniformLocation("texNormal", true),
		GL_TEXTURE1
	);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texAlbedoSpec);
	glUniform1i(
		shader.getUniformLocation("texAlbedoSpecular", true),
		GL_TEXTURE2
	);
}

void GBuffer::UnbindTextures()
{
	// texPosition.Unbind(0);
	// texNormal.Unbind(1);
	// texAlbedoSpec.Unbind(2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
}

}
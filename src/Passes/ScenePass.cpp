#include "ScenePass.h"

#include "../GM/RT.h" //Render Target
#include "../GM/Tex2D.h" //2D texture
#include "../Scene/Node.h"

#include <GL3\gl3w.h>

using namespace Pass;

ScenePass::ScenePass(unsigned int width, unsigned int height)
	: IPass(width, height)
{
	fbo = std::make_shared<GM::FBO>(w,h);

	//Add render targets
	fbo->add(GL_COLOR_ATTACHMENT0, std::make_shared<GM::RT>(GL_RGBA8, w,h));				//Diffuse
	fbo->add(GL_DEPTH_ATTACHMENT,  std::make_shared<GM::RT>(GL_DEPTH_COMPONENT24, w,h));	//Depth

	//Add render textures
	fbo->add(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, "TEX_DIFF", 
			std::make_shared<GM::Tex2D>(GM::T2DTexParams(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 4, w,h))); //Diffuse

	//Check that everything is ok
	fbo->check();
	fbo->unbind();
}

ScenePass::~ScenePass()
{
}

void ScenePass::begin()
{
	fbo->bind();
    glClearColor(0.9f,0.85f,0.7f,1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	GLint nViewport[4];
	glGetIntegerv(GL_VIEWPORT, nViewport);
	temp_w = nViewport[2];
	temp_h = nViewport[3];
	glViewportIndexedf(0,0,0,(float)w,(float)h);
	//glViewport(0,0,w,h);
	glActiveTexture(GL_TEXTURE0);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);
}

void ScenePass::end()
{
	fbo->unbind();
	glViewportIndexedf(0,0,0,(float)temp_w,(float)temp_h);
}

void ScenePass::bind(unsigned int active_program, unsigned int index_offset)
{
	fbo->bind_rt(active_program, index_offset);
}

void ScenePass::unbind(unsigned int index_offset)
{
	fbo->unbind_rt(index_offset);
}

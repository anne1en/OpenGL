#include "BloomPass.h"

#include "../GM/RT.h" //Render Target
#include "../GM/Tex2D.h" //2D texture
#include "../Scene/Node.h"
#include "../Kernel.h"
#include "../File/ShaderLoader.h"

#include <GL3\gl3w.h>

using namespace Pass;

BloomPass::BloomPass(unsigned int width, unsigned int height)
	: IPass(width, height), internal_pass(false)
{
	bloom_color_highlight_pass = std::make_shared<BloomPass>(width, height, "bloom_pass_color_highpass.fs");
	bloom_gaussian_blur_horizontal_pass = std::make_shared<BloomPass>(width, height, "bloom_pass_gaussian_blur_horizontal.fs");
	bloom_gaussian_blur_vertical_pass = std::make_shared<BloomPass>(width, height, "bloom_pass_gaussian_blur_vertical.fs");

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

	quad = std::make_shared<Scene::Quad>();
	shader = Kernel::getSingleton()->getShaderLoader()->load("fullscreen_pass.vs", std::string(), "bloom_pass.fs");
}

BloomPass::BloomPass(unsigned int width, unsigned int height, const std::string &fragment_shader)
	: IPass(width, height), internal_pass(true)
{
	fbo = std::make_shared<GM::FBO>(w,h);

	//Add render targets
	fbo->add(GL_COLOR_ATTACHMENT0, std::make_shared<GM::RT>(GL_RGBA8, w,h));				//Diffuse
	fbo->add(GL_DEPTH_ATTACHMENT,  std::make_shared<GM::RT>(GL_DEPTH_COMPONENT24, w,h));	//Depth

	//Add render textures
	fbo->add(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, "TEX_BLOOM", 
			std::make_shared<GM::Tex2D>(GM::T2DTexParams(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 4, w,h))); //Diffuse

	//Check that everything is ok
	fbo->check();
	fbo->unbind();

	quad = std::make_shared<Scene::Quad>();
	shader = Kernel::getSingleton()->getShaderLoader()->load("fullscreen_pass.vs", std::string(), fragment_shader);
}

BloomPass::~BloomPass()
{
}

void BloomPass::begin()
{
	if(internal_pass)
		internalBegin();
}

void BloomPass::end()
{
	if(internal_pass)
		internalEnd();
}

void BloomPass::internalBegin()
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

void BloomPass::internalEnd()
{
	fbo->unbind();
	glViewportIndexedf(0,0,0,(float)temp_w,(float)temp_h);
}

void BloomPass::render(const IPassPtr &prev_pass)
{
	bloom_color_highlight_pass->begin();
	{
		bloom_color_highlight_pass->render(prev_pass, quad);
	}bloom_color_highlight_pass->end();

	bloom_gaussian_blur_horizontal_pass->begin();
	{
		bloom_gaussian_blur_horizontal_pass->render(bloom_color_highlight_pass, quad);
	} bloom_gaussian_blur_horizontal_pass->end();
	
	bloom_gaussian_blur_vertical_pass->begin();
	{
		bloom_gaussian_blur_vertical_pass->render(bloom_gaussian_blur_horizontal_pass, quad);
	}bloom_gaussian_blur_vertical_pass->end();

	internalBegin();
	{
		shader->bind();
		{
			//Bind the fullscreen texture of the previous pass
			prev_pass->bind(shader->getFS(), 0);
			//Bind the fullscreen texture of the last bloom filter blur pass
			bloom_gaussian_blur_vertical_pass->bind(shader->getFS(), 1);

			//Then render the fullscreen quad of this pass, which
			//will be drawn onto the Bloom FBO texture.
			quad->render();
		} shader->unbind();
	} internalEnd();
}

void BloomPass::render(const IPassPtr &prev_pass, const Scene::QuadPtr &quad)
{
	shader->bind();
	{
		//Bind the fullscreen texture of the previous pass
		prev_pass->bind(shader->getFS(), 0);

		//Then render the fullscreen quad of this pass, which
		//will be drawn onto the back buffer
		quad->render();

	} shader->unbind();
}

void BloomPass::bind(unsigned int active_program, unsigned int index_offset)
{
	fbo->bind_rt(active_program, index_offset);
}

void BloomPass::unbind(unsigned int index_offset)
{
	fbo->unbind_rt(index_offset);
}

#include "GodraysPass.h"

#include "../GM/RT.h" //Render Target
#include "../GM/Tex2D.h" //2D texture
#include "../Scene/Node.h"
#include "../Scene/Camera.h"
#include "../Scene/Light.h"
#include "../Kernel.h"
#include "../File/ShaderLoader.h"

#include <GL3\gl3w.h>

using namespace Pass;

GodraysPass::GodraysPass(unsigned int width, unsigned int height)
	: IPass(width, height), internal_pass(false)
{
	godrays_occlusion_pass = std::make_shared<GodraysPass>(width, height, "godrays_occlusion_pass.fs"); //The shader at the end is a bit misleading, as it's not in use...
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
	shader = Kernel::getSingleton()->getShaderLoader()->load("fullscreen_pass.vs", std::string(), "godrays_pass.fs");
	uni_lightPosition_cs = std::make_shared<GM::Uniform>(shader->getFS(), "lightPosition_cs");
}

GodraysPass::GodraysPass(unsigned int width, unsigned int height, const std::string &fragment_shader)
	: IPass(width, height), internal_pass(true)
{
	fbo = std::make_shared<GM::FBO>(w,h);

	//Add render targets
	fbo->add(GL_COLOR_ATTACHMENT0, std::make_shared<GM::RT>(GL_RGBA8, w,h));				//Diffuse
	fbo->add(GL_DEPTH_ATTACHMENT,  std::make_shared<GM::RT>(GL_DEPTH_COMPONENT24, w,h));	//Depth

	//Add render textures
	fbo->add(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, "TEX_OCCLUSION", 
			std::make_shared<GM::Tex2D>(GM::T2DTexParams(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 4, w,h))); //Diffuse

	//Check that everything is ok
	fbo->check();
	fbo->unbind();

	quad = std::make_shared<Scene::Quad>();
	shader = Kernel::getSingleton()->getShaderLoader()->load("fullscreen_pass.vs", std::string(), fragment_shader);
}

GodraysPass::~GodraysPass()
{
}

void GodraysPass::beginOcclusion()
{
	godrays_occlusion_pass->begin();
}

void GodraysPass::endOcclusion()
{
	godrays_occlusion_pass->end();
}

void GodraysPass::begin()
{
	if(internal_pass)
		internalBegin();
}

void GodraysPass::end()
{
	if(internal_pass)
		internalEnd();
}

void GodraysPass::internalBegin()
{
	fbo->bind();
    glClearColor(0.0f,0.0f,0.2f,0.0f);
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

void GodraysPass::internalEnd()
{
	fbo->unbind();
	glViewportIndexedf(0,0,0,(float)temp_w,(float)temp_h);
}

void GodraysPass::render(const IPassPtr &prev_pass)
{
	/*godrays_occlusion_pass->begin();
	{
		godrays_occlusion_pass->render(prev_pass, quad);
	} godrays_occlusion_pass->end();*/
	internalBegin();
	{
		shader->bind();
		{
			auto camera = Scene::Camera::getSingleton();
			auto world_to_view = camera->getWorldToViewMatrix();
			auto view_to_clip  = camera->getViewToClipMatrix();
			auto light = Scene::Node::getLight(0);
			auto lightPosition_cs = (view_to_clip * world_to_view * glm::vec4(light->getPosition(), 1.0f));
			lightPosition_cs.x /= lightPosition_cs.w;
			lightPosition_cs.y /= lightPosition_cs.w;
			lightPosition_cs.z /= lightPosition_cs.w;

			uni_lightPosition_cs->bind(glm::vec2(lightPosition_cs));
			//Bind the fullscreen texture of the previous pass
			prev_pass->bind(shader->getFS(), 0);
			//Bind the fullscreen texture of the last bloom filter blur pass
			godrays_occlusion_pass->bind(shader->getFS(), 1);

			//Then render the fullscreen quad of this pass, which
			//will be drawn onto the Bloom FBO texture.
			quad->render();
		} shader->unbind();
	} internalEnd();
}

void GodraysPass::render(const IPassPtr &prev_pass, const Scene::QuadPtr &quad)
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

void GodraysPass::bind(unsigned int active_program, unsigned int index_offset)
{
	fbo->bind_rt(active_program, index_offset);
}

void GodraysPass::unbind(unsigned int index_offset)
{
	fbo->unbind_rt(index_offset);
}

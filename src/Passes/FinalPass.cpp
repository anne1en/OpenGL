#include "FinalPass.h"

#include "../GM/RT.h" //Render Target
#include "../GM/Tex2D.h" //2D texture
#include "../Scene/Node.h"
#include "../Kernel.h"
#include "../File/ShaderLoader.h"

#include <GL3\gl3w.h>

using namespace Pass;

FinalPass::FinalPass(unsigned int width, unsigned int height)
	: IPass(width, height)
{
	quad = std::make_shared<Scene::Quad>();
	shader = Kernel::getSingleton()->getShaderLoader()->load("fullscreen_pass.vs", std::string(), "final_pass.fs");
}

FinalPass::~FinalPass()
{
}

void FinalPass::begin()
{
}

void FinalPass::end()
{
}

void FinalPass::render(const IPassPtr &prev_pass)
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

#include "IPass.h"

#include "../GM/RT.h" //Render Target
#include "../GM/Tex2D.h" //2D texture
#include "../Scene/Node.h"

#include <GL3\gl3w.h>

using namespace Pass;

IPass::IPass(unsigned int width, unsigned int height)
	: w(width), h(height), temp_w(0), temp_h(0)
{
}

IPass::~IPass()
{
}

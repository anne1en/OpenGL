#pragma once

#include "IPass.h"

#include "../Scene/Quad.h"

#include "../GM/FBO.h"
#include "../GM/Shader.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <functional>
#include <vector>

//////////////////////////////////////////////
// Written by Paal Trefall
// Free for all use, but at your own risk!
//////////////////////////////////////////////
namespace Pass
{
class FinalPass; typedef std::shared_ptr<FinalPass> FinalPassPtr;
class FinalPass : public IPass
{
public:
	FinalPass(unsigned int width, unsigned int height);
	virtual ~FinalPass();

	void begin() override;
	void end() override;

	void render(const IPassPtr &prev_pass) override;

protected:
	Scene::QuadPtr quad;
	GM::ShaderPtr shader;
};
}

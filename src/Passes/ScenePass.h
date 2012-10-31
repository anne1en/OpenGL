#pragma once

#include "IPass.h"

#include "../GM/FBO.h"

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
class ScenePass; typedef std::shared_ptr<ScenePass> ScenePassPtr;
class ScenePass : public IPass
{
public:
	ScenePass(unsigned int width, unsigned int height);
	virtual ~ScenePass();

	void begin() override;
	void end() override;

	void bind(unsigned int active_program, unsigned int index_offset) override;
	void unbind(unsigned int index_offset) override;

protected:
	GM::FBOPtr fbo;
};
}

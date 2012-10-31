#pragma once

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
class IPass; typedef std::shared_ptr<IPass> IPassPtr;
class IPass
{
public:
	IPass(unsigned int width, unsigned int height);
	virtual ~IPass();

	virtual void begin() = 0;
	virtual void end() = 0;

	virtual void render(const IPassPtr &prev_pass) {}

	virtual void bind(unsigned int active_program, unsigned int index_offset) {}
	virtual void unbind(unsigned int index_offset) {}

protected:
	unsigned int w,h, temp_w,temp_h;
};
}

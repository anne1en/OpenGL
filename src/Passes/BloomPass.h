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
class BloomPass; typedef std::shared_ptr<BloomPass> BloomPassPtr;
class BloomPass : public IPass
{
public:
	BloomPass(unsigned int width, unsigned int height);
	BloomPass(unsigned int width, unsigned int height, const std::string &fragment_shader);
	virtual ~BloomPass();

	void begin() override;
	void end() override;

	void render(const IPassPtr &prev_pass) override;
	void render(const IPassPtr &prev_pass, const Scene::QuadPtr &quad);

	void bind(unsigned int active_program, unsigned int index_offset) override;
	void unbind(unsigned int index_offset) override;

protected:
	void internalBegin();
	void internalEnd();
	BloomPassPtr bloom_color_highlight_pass;
	BloomPassPtr bloom_gaussian_blur_horizontal_pass;
	BloomPassPtr bloom_gaussian_blur_vertical_pass;

	GM::FBOPtr fbo;
	bool internal_pass;

	Scene::QuadPtr quad;
	GM::ShaderPtr shader;
};
}
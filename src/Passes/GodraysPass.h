#pragma once

#include "IPass.h"

#include "../Scene/Quad.h"

#include "../GM/FBO.h"
#include "../GM/Shader.h"
#include "../GM/Uniform.h"

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
class GodraysPass; typedef std::shared_ptr<GodraysPass> GodraysPassPtr;
class GodraysPass : public IPass
{
public:
	GodraysPass(unsigned int width, unsigned int height);
	GodraysPass(unsigned int width, unsigned int height, const std::string &fragment_shader);
	virtual ~GodraysPass();

	void beginOcclusion();
	void endOcclusion();

	void begin() override;
	void end() override;

	void render(const IPassPtr &prev_pass) override;
	void render(const IPassPtr &prev_pass, const Scene::QuadPtr &quad);

	void bind(unsigned int active_program, unsigned int index_offset) override;
	void unbind(unsigned int index_offset) override;

protected:
	void internalBegin();
	void internalEnd();

	GodraysPassPtr godrays_occlusion_pass;

	GM::FBOPtr fbo;
	bool internal_pass;

	Scene::QuadPtr quad;
	GM::ShaderPtr shader;
	GM::UniformPtr uni_lightPosition_cs;

};
}
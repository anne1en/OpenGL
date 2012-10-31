#pragma once

#include <GL3\gl3w.h>
#include <glm/glm.hpp>

#include <memory>
#include <string>

namespace GM
{
	class Sampler;
	typedef std::shared_ptr<Sampler> SamplerPtr;

	class Sampler
	{
	public:
		Sampler(int wrap_mode = GL_CLAMP_TO_EDGE);
		~Sampler();
		void bind(unsigned int unit = 0);
		void unbind(unsigned int unit = 0);

		void setParameteri(unsigned int parameter, int value);
		void setParameterf(unsigned int parameter, float value);
		void setParameterfv(unsigned int parameter, float *value);

	private:
		unsigned int handle;
	};
}
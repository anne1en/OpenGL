#pragma once

#include "Node.h"
#include "../GM/Uniform.h"
#include "../GM/Shader.h"

#include <glm/glm.hpp>
#include <memory>

namespace Scene
{
	class Light; typedef std::shared_ptr<Light> LightPtr;
	class Light : public Node
	{
	public:
		Light();

		struct Data
		{
			unsigned int lightId;
			glm::vec3 position_vs;
			//Expand this later with more light data, like diffuse, range falloff, etc
			Data(unsigned int lightId) : lightId(lightId){}
		};
		typedef std::unique_ptr<Data> DataPtr;

		void bind(const GM::ShaderPtr &active_program, int location);
		void unbind();

		int findLocation(unsigned int program);


	private:
		DataPtr data;

		GM::UniformPtr uni_position_vs;
	};
}

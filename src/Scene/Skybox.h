#pragma once

#include "Node.h"

#include "../GM/VAO.h"
#include "../GM/VBO.h"
#include "../GM/IBO.h"

#include "../GM/TexCUBE.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Scene
{
	class Skybox;
	typedef std::shared_ptr<Skybox> SkyboxPtr;

	class Skybox : public Node
	{
	public:
		Skybox(GM::TexCUBEPtr cube_map, float size = 1000.0f);

		void localRender() override;
		void localRenderOcclusion() override;

	protected:
		void initIndices(std::vector<unsigned int> &indices);
		void initVertices(std::vector<float> &vertices, float size);
		void initTexCoords(std::vector<float> &texCoords);

		GM::VAOPtr vao;
		GM::VBOPtr vbo;
		GM::IBOPtr ibo;

		GM::TexCUBEPtr cube_map;
	};
}

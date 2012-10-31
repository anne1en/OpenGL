#pragma once

#include "../GM/VAO.h"
#include "../GM/VBO.h"
#include "../GM/IBO.h"

#include "../GM/Shader.h"

#include <memory>

namespace Scene
{
	class Quad;
	typedef std::shared_ptr<Quad> QuadPtr;

	class Quad
	{
	public:
		Quad();
		void render();

	private:
		GM::VAOPtr vao;
		GM::VBOPtr vbo;
		GM::IBOPtr ibo;
	};
}

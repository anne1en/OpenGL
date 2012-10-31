#pragma once

#include "Node.h"
#include "MeshData_t.h"

#include "../GM/VAO.h"
#include "../GM/VBO.h"
#include "../GM/IBO.h"

#include <glm/glm.hpp>
#include <memory>

namespace Scene
{
	class Mesh;
	typedef std::shared_ptr<Mesh> MeshPtr;

	class Mesh : public Node
	{
	public:
		Mesh(MeshDataPtr data);

		void localRender() override;
		void localRenderOcclusion() override;
		void localUpdate(float dt) override;

		GM::VAOPtr getVao(){return vao;}
		GM::VBOPtr getVbo(){return vbo;}
		GM::IBOPtr getIbo(){return ibo;}

	protected:
		GM::VAOPtr vao;
		GM::VBOPtr vbo;
		GM::IBOPtr ibo;
	};
}

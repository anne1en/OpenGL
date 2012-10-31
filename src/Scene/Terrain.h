#pragma once

#include "Node.h"

#include "../GM/VAO.h"
#include "../GM/VBO.h"
#include "../GM/IBO.h"

#include <glm/glm.hpp>
#include <memory>

namespace Scene
{
	class Terrain;
	typedef std::shared_ptr<Terrain> TerrainPtr;

	class Terrain : public Node
	{
	public:
		Terrain(GM::Tex2DPtr heightmap, const std::string &heightmap_sampler_name, float spacing, float height_mod);

		void localRender() override;
		void localRenderOcclusion() override;
		void localUpdate(float dt) override;

		GM::VAOPtr getVao(){return vao;}
		GM::VBOPtr getVbo(){return vbo;}
		GM::IBOPtr getIbo(){return ibo;}

		const glm::vec2 &getTerrainSize() const { return terrain_size; }

		float getHeight(const glm::vec3 &position) const;

	protected:
		void buildIndices(std::vector<unsigned int> &indices, unsigned int x, unsigned int y, float z, unsigned int width, unsigned int height);
		void buildVertices(std::vector<float> &vertices, unsigned int x, unsigned int y, float z, float spacing, float height_mod);
		void buildTexCoords(std::vector<float> &texCoords, unsigned int x, unsigned int y, float z, unsigned int width, unsigned int height);

		void generateNormals(std::vector<float> &normals, const std::vector<float> &vertices, unsigned int width, unsigned int height);

		GM::VAOPtr vao;
		GM::VBOPtr vbo;
		GM::IBOPtr ibo;

		glm::vec2 terrain_size;
		std::vector<glm::vec3> pos;
	};
}

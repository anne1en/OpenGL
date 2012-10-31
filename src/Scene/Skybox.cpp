#include "Skybox.h"
#include "Camera.h"
#include "Light.h"

#include "../GM/ATTRIB.h"
#include "../GM/ShaderConstants.h"

#include "../Kernel.h"

#include <glm/ext.hpp>

#include <memory>
#include <vector>

using namespace Scene;
using namespace glm;

Skybox::Skybox(GM::TexCUBEPtr cube_map, float size)
	: Node("skybox.vs", "skybox.fs", "skybox.vs", "occlusion_light.fs"), cube_map(cube_map)
{
	std::vector<unsigned int> indices;
	std::vector<float> vertices;
	std::vector<float> texCoords;

	initIndices(indices);
	initVertices(vertices, size);
	initTexCoords(texCoords);

	unsigned int buffer_size = sizeof(float) * (vertices.size() + texCoords.size());

	vao = std::make_shared<GM::VAO>();
	vbo = std::make_shared<GM::VBO>(buffer_size, GL_STATIC_DRAW);
	ibo = std::make_shared<GM::IBO>(indices, GL_STATIC_DRAW);

	auto v_offset = vbo->buffer<float>(vertices);
	auto t_offset = vbo->buffer<float>(texCoords);

	GM::ATTRIB::bind(GM::ShaderConstants::Position(), 3, GL_FLOAT, false, 0, v_offset);
	GM::ATTRIB::bind(GM::ShaderConstants::TexCoord(), 3, GL_FLOAT, false, 0, t_offset);

	vao->unbind();
	vbo->unbind();
	ibo->unbind();
}

void Skybox::localRender()
{
	shader->bind();

	bindCommonUniforms();
	glActiveTexture(GL_TEXTURE0);
	cube_map->bind();

	vao->bind();

	glDrawElements(GL_TRIANGLES, ibo->size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	
	cube_map->unbind();
	shader->unbind();
}

void Skybox::localRenderOcclusion()
{
	shader_occlusion->bind();

	auto camera = Camera::getSingleton();
	uni_otw_occlusion->bind(object_to_world);
	uni_wtv_occlusion->bind(camera->getWorldToViewMatrix());
	uni_vtc_occlusion->bind(camera->getViewToClipMatrix());
	glActiveTexture(GL_TEXTURE0);
	cube_map->bind();

	vao->bind();

	glDrawElements(GL_TRIANGLES, ibo->size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	shader_occlusion->unbind();
}

void Skybox::initIndices(std::vector<unsigned int> &indices)
{
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(2); indices.push_back(3); indices.push_back(0);

	indices.push_back(4); indices.push_back(5); indices.push_back(6);
	indices.push_back(6); indices.push_back(7); indices.push_back(4);

	indices.push_back(8); indices.push_back(9); indices.push_back(10);
	indices.push_back(10); indices.push_back(11); indices.push_back(8);

	indices.push_back(12); indices.push_back(13); indices.push_back(14);
	indices.push_back(14); indices.push_back(15); indices.push_back(12);

	indices.push_back(16); indices.push_back(17); indices.push_back(18);
	indices.push_back(18); indices.push_back(19); indices.push_back(16);

	indices.push_back(20); indices.push_back(21); indices.push_back(22);
	indices.push_back(22); indices.push_back(23); indices.push_back(20);
}

void Skybox::initVertices(std::vector<float> &vertices, float size)
{
	//BACK
	vertices.push_back(size); vertices.push_back(-size); vertices.push_back(-size);
	vertices.push_back(size); vertices.push_back(size); vertices.push_back(-size);
	vertices.push_back(-size); vertices.push_back(size); vertices.push_back(-size);
	vertices.push_back(-size); vertices.push_back(-size); vertices.push_back(-size);

	//BOTTOM
	vertices.push_back(-size); vertices.push_back(-size); vertices.push_back(-size);
	vertices.push_back(-size); vertices.push_back(-size); vertices.push_back(size);
	vertices.push_back(size); vertices.push_back(-size); vertices.push_back(size);
	vertices.push_back(size); vertices.push_back(-size); vertices.push_back(-size);

	//FRONT
	vertices.push_back(-size); vertices.push_back(-size); vertices.push_back(size);
	vertices.push_back(-size); vertices.push_back(size); vertices.push_back(size);
	vertices.push_back(size); vertices.push_back(size); vertices.push_back(size);
	vertices.push_back(size); vertices.push_back(-size); vertices.push_back(size);

	//LEFT
	vertices.push_back(-size); vertices.push_back(size); vertices.push_back(-size);
	vertices.push_back(-size); vertices.push_back(size); vertices.push_back(size);
	vertices.push_back(-size); vertices.push_back(-size); vertices.push_back(size);
	vertices.push_back(-size); vertices.push_back(-size); vertices.push_back(-size);

	//RIGHT
	vertices.push_back(size); vertices.push_back(-size); vertices.push_back(-size);
	vertices.push_back(size); vertices.push_back(-size); vertices.push_back(size);
	vertices.push_back(size); vertices.push_back(size); vertices.push_back(size);
	vertices.push_back(size); vertices.push_back(size); vertices.push_back(-size);

	//TOP
	vertices.push_back(size); vertices.push_back(size); vertices.push_back(-size);
	vertices.push_back(size); vertices.push_back(size); vertices.push_back(size);
	vertices.push_back(-size); vertices.push_back(size); vertices.push_back(size);
	vertices.push_back(-size); vertices.push_back(size); vertices.push_back(-size);
}

void Skybox::initTexCoords(std::vector<float> &texCoords)
{
	//BACK
	texCoords.push_back(1.0f); texCoords.push_back(1.0f); texCoords.push_back(-1.0f);
	texCoords.push_back(1.0f); texCoords.push_back(-1.0f); texCoords.push_back(-1.0f);
	texCoords.push_back(-1.0f); texCoords.push_back(-1.0f); texCoords.push_back(-1.0f);
	texCoords.push_back(-1.0f); texCoords.push_back(1.0f); texCoords.push_back(-1.0f);

	//BOTTOM
	texCoords.push_back(-1.0f); texCoords.push_back(-1.0f); texCoords.push_back(1.0f);
	texCoords.push_back(-1.0f); texCoords.push_back(-1.0f); texCoords.push_back(-1.0f);
	texCoords.push_back(1.0f); texCoords.push_back(-1.0f); texCoords.push_back(-1.0f);
	texCoords.push_back(1.0f); texCoords.push_back(-1.0f); texCoords.push_back(1.0f);

	//FRONT
	texCoords.push_back(-1.0f); texCoords.push_back(1.0f); texCoords.push_back(1.0f);
	texCoords.push_back(-1.0f); texCoords.push_back(-1.0f); texCoords.push_back(1.0f);
	texCoords.push_back(1.0f); texCoords.push_back(-1.0f); texCoords.push_back(1.0f);
	texCoords.push_back(1.0f); texCoords.push_back(1.0f); texCoords.push_back(1.0f);

	//LEFT
	texCoords.push_back(-1.0f); texCoords.push_back(-1.0f); texCoords.push_back(-1.0f);
	texCoords.push_back(-1.0f); texCoords.push_back(-1.0f); texCoords.push_back(1.0f);
	texCoords.push_back(-1.0f); texCoords.push_back(1.0f); texCoords.push_back(1.0f);
	texCoords.push_back(-1.0f); texCoords.push_back(1.0f); texCoords.push_back(-1.0f);

	//RIGHT
	texCoords.push_back(1.0f); texCoords.push_back(1.0f); texCoords.push_back(-1.0f);
	texCoords.push_back(1.0f); texCoords.push_back(1.0f); texCoords.push_back(1.0f);
	texCoords.push_back(1.0f); texCoords.push_back(-1.0f); texCoords.push_back(1.0f);
	texCoords.push_back(1.0f); texCoords.push_back(-1.0f); texCoords.push_back(-1.0f);

	//TOP
	texCoords.push_back(1.0f); texCoords.push_back(1.0f); texCoords.push_back(1.0f);
	texCoords.push_back(1.0f); texCoords.push_back(1.0f); texCoords.push_back(-1.0f);
	texCoords.push_back(-1.0f); texCoords.push_back(1.0f); texCoords.push_back(-1.0f);
	texCoords.push_back(-1.0f); texCoords.push_back(1.0f); texCoords.push_back(1.0f);
}
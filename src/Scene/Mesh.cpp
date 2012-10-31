#include "Mesh.h"
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

Mesh::Mesh(MeshDataPtr data)
	: Node("pass_through_simple.vs", "pass_through_simple.fs")
{
	vao = std::make_shared<GM::VAO>();
	vbo = std::make_shared<GM::VBO>(data->getBufferSize(), GL_STATIC_DRAW);
	ibo = std::make_shared<GM::IBO>(data->indices, GL_STATIC_DRAW);

	bool normals_loaded = false;
	bool tangents_loaded = false;
	bool bitangents_loaded = false;
	bool texcoords_loaded = false;
	bool colors_loaded = false;

	std::vector<unsigned int> offsets;
	offsets.push_back(vbo->buffer<float>(data->vertices));
	if(data->hasNormals())		offsets.push_back(vbo->buffer<float>(data->normals));
	if(data->hasTangents())		offsets.push_back(vbo->buffer<float>(data->tangents));
	if(data->hasBitangents())	offsets.push_back(vbo->buffer<float>(data->bitangents));
	if(data->hasTexCoords())	offsets.push_back(vbo->buffer<float>(data->texcoords));
	if(data->hasColors())		offsets.push_back(vbo->buffer<float>(data->colors));

	GM::ATTRIB::bind(GM::ShaderConstants::Position(), 3, GL_FLOAT, false, 0, offsets[0]);
	for(unsigned int i = 1; i < offsets.size(); i++)
	{
		if(data->hasNormals() && !normals_loaded) 
		{
			GM::ATTRIB::bind(GM::ShaderConstants::Normal(), 3, GL_FLOAT, false, 0, offsets[i]);
			normals_loaded = true;
		}
		else if(data->hasTangents() && !tangents_loaded) 
		{
			GM::ATTRIB::bind(GM::ShaderConstants::Tangent(), 3, GL_FLOAT, false, 0, offsets[i]);
			tangents_loaded = true;
		}
		else if(data->hasBitangents() && !bitangents_loaded) 
		{
			GM::ATTRIB::bind(GM::ShaderConstants::Bitangent(), 3, GL_FLOAT, false, 0, offsets[i]);
			bitangents_loaded = true;
		}
		else if(data->hasTexCoords() && !texcoords_loaded) 
		{
			GM::ATTRIB::bind(GM::ShaderConstants::TexCoord(), 2, GL_FLOAT, false, 0, offsets[i]);
			texcoords_loaded = true;
		}
		else if(data->hasColors() && !colors_loaded) 
		{
			GM::ATTRIB::bind(GM::ShaderConstants::Diffuse(), 4, GL_FLOAT, false, 0, offsets[i]);
			colors_loaded = true;
		}
	}

	vao->unbind();
	vbo->unbind();
	ibo->unbind();
}

void Mesh::localRender()
{
	shader->bind();

	bindCommonUniforms();
	bindLights();
	bindTextures();

	vao->bind();

	glDrawElements(GL_TRIANGLES, ibo->size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	shader->unbind();
}

void Mesh::localRenderOcclusion()
{
	shader_occlusion->bind();

	auto camera = Camera::getSingleton();
	uni_otw_occlusion->bind(object_to_world);
	uni_wtv_occlusion->bind(camera->getWorldToViewMatrix());
	uni_vtc_occlusion->bind(camera->getViewToClipMatrix());
	
	vao->bind();

	glDrawElements(GL_TRIANGLES, ibo->size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	shader_occlusion->unbind();
}

void Mesh::localUpdate(float dt)
{
	rotate(dt*10.0f, vec3(0,1,0));
}

#include "Terrain.h"
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

Terrain::Terrain(GM::Tex2DPtr heightmap, const std::string &heightmap_sampler_name, float spacing, float height_mod)
	: Node("terrain.vs", "terrain.fs")
{
	addTexture(heightmap, heightmap_sampler_name);

	std::vector<unsigned int> indices;
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;

	unsigned int width = heightmap->getWidth();
	unsigned int height = heightmap->getHeight();
	unsigned char *data = heightmap->getData();

	terrain_size = vec2(width*spacing, height*spacing);

	float z = 0.0f;
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	unsigned int dpp = heightmap->getBpp();
	int aux = 0;
	unsigned int step = 1;

	unsigned int base_alloc = (width/step)*(height/step);
	vertices.reserve(	base_alloc * 3 );
	texCoords.reserve(	base_alloc * 2 );

	for(unsigned int y = 0; y < height; y += step)
	for(unsigned int x = 0; x < width; x += step)
	{
		aux = dpp*(y*width + x);
		//if(aux > data_vec.size() - 3)
		//	continue;

		/*if(dpp == 4)
			z = data[aux+3] / 256.0f;*/
		z = data[aux+0];
		g = data[aux+1];
		b = data[aux+2];

		buildIndices(indices, x,y,z, width, height);
		buildVertices(vertices, x,y,z, spacing, height_mod);
		buildTexCoords(texCoords, x,y,z, width, height);
	}

	//Convert vertices list to actual positions
	for(unsigned int i = 0; i < vertices.size(); i += 3)
	{
		pos.push_back(vec3(vertices[i+0], vertices[i+1], vertices[i+2]));
	}

	generateNormals(normals, vertices, width, height);

	unsigned int buffer_size = sizeof(float) * (vertices.size() + normals.size() + texCoords.size());

	vao = std::make_shared<GM::VAO>();
	vbo = std::make_shared<GM::VBO>(buffer_size, GL_STATIC_DRAW);
	ibo = std::make_shared<GM::IBO>(indices, GL_STATIC_DRAW);

	auto v_offset = vbo->buffer<float>(vertices);
	auto n_offset = vbo->buffer<float>(normals);
	auto t_offset = vbo->buffer<float>(texCoords);

	GM::ATTRIB::bind(GM::ShaderConstants::Position(), 3, GL_FLOAT, false, 0, v_offset);
	GM::ATTRIB::bind(GM::ShaderConstants::Normal(), 3, GL_FLOAT, false, 0, n_offset);
	GM::ATTRIB::bind(GM::ShaderConstants::TexCoord(), 2, GL_FLOAT, false, 0, t_offset);

	vao->unbind();
	vbo->unbind();
	ibo->unbind();
}

void Terrain::localRender()
{
	shader->bind();

	bindCommonUniforms();
	bindLights();
	bindTextures();

	vao->bind();

	glDrawElements(GL_TRIANGLE_STRIP, ibo->size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	shader->unbind();
}

void Terrain::localRenderOcclusion()
{
	shader_occlusion->bind();

	auto camera = Camera::getSingleton();
	uni_otw_occlusion->bind(object_to_world);
	uni_wtv_occlusion->bind(camera->getWorldToViewMatrix());
	uni_vtc_occlusion->bind(camera->getViewToClipMatrix());
	
	vao->bind();

	glDrawElements(GL_TRIANGLE_STRIP, ibo->size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	shader_occlusion->unbind();
}

void Terrain::localUpdate(float dt)
{
	//rotate(dt*10.0f, vec3(0,1,0));
}

void Terrain::buildIndices(std::vector<unsigned int> &indices, unsigned int x, unsigned int y, float z, unsigned int width, unsigned int height)
{
	// 0	1	2	3
	// 4	5	6	7
	// 8	9	10	c
	// n	13	14	u
	// 16	17	18	19

	//convert the (x,y) to a single index value which represents the "current" index in the triangle strip
	//represented by "c" in the figure above
	unsigned int current_index = y*width+x;

	//find the vertex index in the grid directly under the current index
	//represented by "u" in the figure above
	unsigned int under_index = 0;
	if(y < height-1)
		under_index = (y+1)*width+x;
	else
		return; //This is the last row, which has already been covered in the previous row with triangle strips in mind

	indices.push_back(current_index);
	indices.push_back(under_index);

	//degenerate triangle technique at end of each row, so that we only have one dip call for the entire grid
	//otherwise we'd need one triangle strip per row. We only want one triangle strip for the entire grid!
	if(x < width-1)
		return;

	if(y >= height-2)
		return;

	//find the next vertex index in the grid from the current index
	//represented by "n" in the figure above
	//We already know that the current index is the last in the current row and that it's not the last row in the grid,
	//so no need to make any if-checks here.
	unsigned int next_index = (y+1)*width;

	//Add an invisible degeneration here to bind with next row in triangle strip
	indices.push_back(under_index);
	indices.push_back(next_index);
}

void Terrain::buildVertices(std::vector<float> &vertices, unsigned int x, unsigned int y, float z, float spacing, float height_mod)
{
	//Add one x,y,z vertex for each x,y in the grid
	vertices.push_back((float)x*spacing);
	vertices.push_back(z*height_mod);
	vertices.push_back((float)y*spacing);
}

void Terrain::buildTexCoords(std::vector<float> &texCoords, unsigned int x, unsigned int y, float z, unsigned int width, unsigned int height)
{
	//Add one u,v texCoord for each x,y in the grid
	float s = (float)x/(float)width;
	float t = (float)y/(float)height;
	texCoords.push_back(s);
	texCoords.push_back(t);
}

//////////////////////////////
//
//       a---b---c
//    ^  |  /|  /|
//    |  |/  |/  |
//    z  d---e---f
//    |  |  /|  /|
//    v  |/  |/  |
//       g---h---i
//
//         <-x->
//
///////////////////////////////
void Terrain::generateNormals(std::vector<float> &normals, const std::vector<float> &vertices, unsigned int width, unsigned int height)
{
	vec3 v1,v2,v3,v4,v5,v6;
	vec3 n,n1,n2,n3,n4,n5,n6;

	for(unsigned int z = 0; z < height; z++)
	for(unsigned int x = 0; x < width; x++)
	{
		// back left corner - 1 tri 2 vertices
		if(z == 0 && x == 0)
		{
			v1 = pos[((z+1)*width + (x))] - pos[((z)*width + (x))];
			v2 = pos[((z)*width + (x+1))] - pos[((z)*width + (x))];
			n = glm::cross(v1,v2);
		}
		// left edge - 3 tri 4 vertices
		else if((z > 0 && z < (height-1)) && x == 0)
		{
			v1 = pos[((z)*width + (x+1))]	- pos[((z)*width + (x))];
			v2 = pos[((z-1)*width + (x+1))] - pos[((z)*width + (x))];
			v3 = pos[((z-1)*width + (x))]	- pos[((z)*width + (x))];
			v4 = pos[((z+1)*width + (x+1))] - pos[((z)*width + (x))];
			n1 = glm::cross(v1,v2); n2 = glm::cross(v2,v3); n3 = glm::cross(v3,v4);
			n = (n1+n2+n3)/3.0f;
		}
		// front left corner - 2 tri 3 vertices
		else if(z == (height-1) && x == 0)
		{
			v1 = pos[((z)*width + (x+1))]	- pos[((z)*width + (x))];
			v2 = pos[((z-1)*width + (x+1))] - pos[((z)*width + (x))];
			v3 = pos[((z-1)*width + (x))]	- pos[((z)*width + (x))];
			n1 = glm::cross(v1,v2); n2 = glm::cross(v2,v3);
			n = (n1+n2)/2.0f;
		}
		// front edge - 3 tri 4 vertices
		else if(z == (height-1) && (x > 0 && x < (width-1)))
		{
			v1 = pos[((z)*width + (x+1))]	- pos[((z)*width + (x))];
			v2 = pos[((z-1)*width + (x+1))] - pos[((z)*width + (x))];
			v3 = pos[((z-1)*width + (x))]	- pos[((z)*width + (x))];
			v4 = pos[((z)*width + (x-1))]	- pos[((z)*width + (x))];
			n1 = glm::cross(v1,v2); n2 = glm::cross(v2,v3); n3 = glm::cross(v3,v4);
			n = (n1+n2+n3)/3.0f;
		}
		// front right corner - 1 tri 2 vertices
		else if(z == (height-1) && x == (width-1))
		{
			v1 = pos[((z-1)*width + (x))] - pos[((z)*width + (x))];
			v2 = pos[((z)*width + (x-1))] - pos[((z)*width + (x))];
			n = glm::cross(v1,v2);
		}
		// right edge - 3 tri 4 vertices
		else if((z > 0 && z < (height-1)) && x == (width-1))
		{
			v1 = pos[((z-1)*width + (x))]	- pos[((z)*width + (x))];
			v2 = pos[((z)*width + (x-1))]	- pos[((z)*width + (x))];
			v3 = pos[((z+1)*width + (x-1))]	- pos[((z)*width + (x))];
			v4 = pos[((z+1)*width + (x))]	- pos[((z)*width + (x))];
			n1 = glm::cross(v1,v2); n2 = glm::cross(v2,v3); n3 = glm::cross(v3,v4);
			n = (n1+n2+n3)/3.0f;
		}
		// back right corner - 2 tri 3 vertices
		else if(z == 0 && x == (width-1))
		{
			v1 = pos[((z)*width + (x-1))]	- pos[((z)*width + (x))];
			v2 = pos[((z+1)*width + (x-1))]	- pos[((z)*width + (x))];
			v3 = pos[((z+1)*width + (x))]	- pos[((z)*width + (x))];
			n1 = glm::cross(v1,v2); n2 = glm::cross(v2,v3);
			n = (n1+n2)/2.0f;
		}
		// back edge - 3 tri 4 vertices
		else if(z == 0 && (x > 0 && x < (width-1)))
		{
			v1 = pos[((z)*width + (x-1))]	- pos[((z)*width + (x))];
			v2 = pos[((z+1)*width + (x-1))]	- pos[((z)*width + (x))];
			v3 = pos[((z+1)*width + (x))]	- pos[((z)*width + (x))];
			v4 = pos[((z)*width + (x+1))]	- pos[((z)*width + (x))];
			n1 = glm::cross(v1,v2); n2 = glm::cross(v2,v3); n3 = glm::cross(v3,v4);
			n = (n1+n2+n3)/3.0f;
		}
		// internal - 6 tri 6 vertices
		else
		{
			v1 = pos[((z)*width + (x+1))]	- pos[((z)*width + (x))];
			v2 = pos[((z-1)*width + (x+1))]	- pos[((z)*width + (x))];
			v3 = pos[((z-1)*width + (x))]	- pos[((z)*width + (x))];
			v4 = pos[((z)*width + (x-1))]	- pos[((z)*width + (x))];
			v5 = pos[((z+1)*width + (x-1))]	- pos[((z)*width + (x))];
			v6 = pos[((z+1)*width + (x))]	- pos[((z)*width + (x))];
			n1 = glm::cross(v1,v2); n2 = glm::cross(v2,v3); n3 = glm::cross(v3,v4);
			n4 = glm::cross(v4,v5); n5 = glm::cross(v5,v6); n6 = glm::cross(v6,v1);
			n = (n1+n2+n3+n4+n5+n6)/6.0f;
		}

		n = glm::normalize(n);
		normals.push_back(n.x);
		normals.push_back(n.y);
		normals.push_back(n.z);
	}
}

float Terrain::getHeight(const glm::vec3 &position) const
{
	auto pos_index = (position.z + (terrain_size.y/2.0f))*terrain_size.x + position.x+(terrain_size.x/2.0f);
	return pos[pos_index].y;
}

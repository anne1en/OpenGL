#include "Node.h"
#include "Camera.h"
#include "Light.h"
#include "../Kernel.h"
#include "../File/ShaderLoader.h"

using namespace Scene;

long long Node::next_uid = 0;
std::vector<LightPtr> Node::lights;

Node::Node(const std::string &vertex_shader, const std::string &fragment_shader, const std::string &occlusion_vertex_shader, const std::string &occlusion_fragment_shader)
	: uid(next_uid++), parent(nullptr), object_to_world(1.0f), position(0.0f), scale(1.0f), orientation(glm::quat(1.f,0.f,0.f,0.f))
{
	if(vertex_shader.empty())
		return;

	auto kernel = Kernel::getSingleton();
	shader = kernel->getShaderLoader()->load(vertex_shader, std::string(), fragment_shader);
	shader_occlusion = kernel->getShaderLoader()->load(occlusion_vertex_shader, std::string(), occlusion_fragment_shader);
	uni_object_to_world	= std::make_shared<GM::Uniform>(shader->getVS(), "Object_to_World");
	uni_world_to_view = std::make_shared<GM::Uniform>(shader->getVS(), "World_to_View");
	uni_view_to_clip = std::make_shared<GM::Uniform>(shader->getVS(), "View_to_Clip");
	uni_normal_to_view = std::make_shared<GM::Uniform>(shader->getVS(), "Normal_to_View");
	uni_camPos_vs = std::make_shared<GM::Uniform>(shader->getFS(), "CamPos_vs");

	uni_otw_occlusion = std::make_shared<GM::Uniform>(shader_occlusion->getVS(), "Object_to_World");
	uni_wtv_occlusion = std::make_shared<GM::Uniform>(shader_occlusion->getVS(), "World_to_View");
	uni_vtc_occlusion = std::make_shared<GM::Uniform>(shader_occlusion->getVS(), "View_to_Clip");
}

Node::~Node()
{
}

void Node::update(float dt)
{
	localUpdate(dt);

	for(auto child = children.begin(); child != children.end(); ++child)
		(*child)->update(dt);
}

void Node::prepare()
{
	auto translate_matrix = glm::translate(position);
	auto rotate_matrix = glm::toMat4(orientation);
	auto scale_matrix = glm::scale(scale);

	if(parent == nullptr)	object_to_world = (translate_matrix*rotate_matrix*scale_matrix);
	else					object_to_world = parent->getObjectToWorldMatrix() * (translate_matrix*rotate_matrix*scale_matrix);

	for(auto child = children.begin(); child != children.end(); ++child)
		(*child)->prepare();
}

void Node::render()
{
	localRender();

	for(auto child = children.begin(); child != children.end(); ++child)
		(*child)->render();
}

void Node::renderOcclusion()
{
	localRenderOcclusion();

	for(auto child = children.begin(); child != children.end(); ++child)
		(*child)->renderOcclusion();
}

void Node::add(const NodePtr &child)
{
	children.push_back(child);
	child->setParent(this);
}

void Node::remove(const NodePtr &child)
{
	for(auto test_child = children.begin(); test_child != children.end(); ++test_child)
	{
		if((*test_child)->getId() == child->getId())
		{
			children.erase(test_child);
			child->setParent(nullptr);
			return;
		}
	}

	std::cout << "Warning: Couldn't find child " << child->getId() << ", asked to be removed from " << uid << "!";
}

void Node::add(const LightPtr &child)
{
	add(std::static_pointer_cast<Node>(child));
	lights.push_back(child);
}

void Node::remove(const LightPtr &child)
{
	remove(std::static_pointer_cast<Node>(child));

	for(auto light = lights.begin(); light != lights.end(); ++light)
	{
		if((*light)->getId() == child->getId())
		{
			lights.erase(light);
			return;
		}
	}

	std::cout << "Warning: Couldn't find light child " << child->getId() << ", asked to be removed from " << uid << "!";
}

void Node::addTexture(const GM::Tex2DPtr &texture, const std::string &sampler_name)
{
	GM::UniformPtr sampler = std::make_shared<GM::Uniform>(shader->getFS(), sampler_name);
	texture_samplers.push_back(sampler);
	textures.push_back(texture);
}

void Node::bindTextures()
{
	for(unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0+i);
		textures[i]->bind();
		texture_samplers[i]->bind((int)i);
	}
}

void Node::bindLights()
{
	//Each node needs to set up it's own uniform location registry for lights, in case they are using different shaders
	//and thus have different uniform locations...
	if(light_uniform_locations.empty())
	{
		for(auto light = lights.begin(); light != lights.end(); ++light)
			light_uniform_locations.push_back((*light)->findLocation(shader->getFS()));
	}

	for(unsigned int i = 0; i < lights.size(); i++)
		lights[i]->bind(shader, light_uniform_locations[i]);
}

void Node::bindCommonUniforms()
{
	auto camera = Camera::getSingleton();

	auto world_to_view = camera->getWorldToViewMatrix();

	//This is the formula for generating the normal to view matrix, needed for normals in shader.
	auto normal_to_view = glm::transpose(glm::inverse(glm::mat3(world_to_view * object_to_world)));

	uni_object_to_world->bind(object_to_world);
	uni_world_to_view->bind(world_to_view);
	uni_view_to_clip->bind( camera->getViewToClipMatrix() );
	uni_normal_to_view->bind(normal_to_view);

	uni_camPos_vs->bind(glm::vec3(world_to_view * glm::vec4(camera->getPos(), 1.0)));
}

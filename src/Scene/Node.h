#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>
#include <memory>

#include "../GM/Tex2D.h"
#include "../GM/Shader.h"
#include "../GM/Uniform.h"

namespace Scene
{

class Light;
typedef std::shared_ptr<Light> LightPtr;

class Node;
typedef std::shared_ptr<Node> NodePtr;

class Node
{
public: 
	Node(const std::string &vertex_shader = std::string(), const std::string &fragment_shader = std::string(), const std::string &occlusion_vertex_shader = "occlusion.vs", const std::string &occlusion_fragment_shader = "occlusion.fs");
	virtual ~Node();

	//Specializers / Overrideables
public:
	virtual void localUpdate(float dt) {}
	virtual void localRender() {}
	virtual void localRenderOcclusion() {}

	//Generics
public:
	const long long &getId() const { return uid; }

	void update(float dt);
	void prepare();
	void render();
	void renderOcclusion();

	void add(const NodePtr &child);
	void remove(const NodePtr &child);
	void add(const LightPtr &child);	//We have some special handling for adding lights
	void remove(const LightPtr &child);
	void setParent(Node *parent) { this->parent = parent; }
	void clearParent() { parent = nullptr; }

	bool hasParent() const { return (parent != nullptr); }
	bool hasChildren() const { return !children.empty(); }

	Node *getParent() const { return parent; }
	const std::vector<NodePtr> &getChildren() const { return children; }

	const glm::mat4 getObjectToWorldMatrix() { return object_to_world; }

	void translate(const glm::vec3 &delta) { this->position += delta; }
	void setPosition(const glm::vec3 &position) { this->position = position; }
	const glm::vec3 &getPosition() const { return position; }

	void setOrientation(const glm::quat &orientation) { this->orientation = orientation; }
	void rotate(float delta_angle, const glm::vec3 &axis) { setOrientation(glm::normalize(glm::angleAxis(delta_angle, axis) * orientation)); }
	void setRotation(float angle, const glm::vec3 &axis) { setOrientation(glm::normalize(glm::angleAxis(angle, axis))); }

	void setScale(const glm::vec3 &scale) { this->scale = scale; }
	void setScale(float scale) { this->scale = glm::vec3(scale); } //uniform scaling

	void addTexture(const GM::Tex2DPtr &texture, const std::string &sampler_name);
	void bindTextures();

	void bindLights();

	void bindCommonUniforms();

	static LightPtr getLight(unsigned int index) { if(index >= lights.size()) return nullptr; return lights[index]; }

	//Common data
protected:
	static long long next_uid; //A pretty shait way of storing a unique id per node.
	long long uid;

	Node *parent;
	std::vector<NodePtr> children;

	glm::mat4 object_to_world;

	glm::vec3 position, scale;
	glm::quat orientation;

	GM::ShaderPtr shader;
	GM::ShaderPtr shader_occlusion;
	GM::UniformPtr uni_object_to_world;
	GM::UniformPtr uni_world_to_view;
	GM::UniformPtr uni_view_to_clip;
	GM::UniformPtr uni_normal_to_view;

	GM::UniformPtr uni_otw_occlusion;
	GM::UniformPtr uni_wtv_occlusion;
	GM::UniformPtr uni_vtc_occlusion;

	GM::UniformPtr uni_camPos_vs;

	std::vector<GM::Tex2DPtr> textures;
	std::vector<GM::UniformPtr> texture_samplers;

	//Common special data
protected:
	static std::vector<LightPtr> lights; //Fairly bad way, but effective, to treat lights in the scene.
	std::vector<int> light_uniform_locations;
};

}

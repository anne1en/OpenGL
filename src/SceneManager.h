#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <functional>
#include <vector>

//////////////////////////////////////////////
// Written by Paal Trefall
// Free for all use, but at your own risk!
//////////////////////////////////////////////
namespace Scene
{
	class Node;
	typedef std::shared_ptr<Node> NodePtr;
}

namespace GM
{
	class Uniform; typedef std::shared_ptr<Uniform> UniformPtr;
}

namespace Pass
{
	class ScenePass; typedef std::shared_ptr<ScenePass> ScenePassPtr;
	class GodraysPass; typedef std::shared_ptr<GodraysPass> GodraysPassPtr;
	class BloomPass; typedef std::shared_ptr<BloomPass> BloomPassPtr;
	class FinalPass; typedef std::shared_ptr<FinalPass> FinalPassPtr;
}

class SceneManager; typedef std::shared_ptr<SceneManager> SceneManagerPtr;
class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	void init(const std::string &resource_dir);

	void update(float dt);
	void render();
	void reshape(int w, int h);

private:
	std::string resource_dir;

	Scene::NodePtr scene;

	Pass::ScenePassPtr scene_pass;
	Pass::GodraysPassPtr godrays_pass;
	Pass::BloomPassPtr bloom_pass;
	Pass::FinalPassPtr final_pass;
};

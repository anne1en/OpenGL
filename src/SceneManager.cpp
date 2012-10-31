#include "SceneManager.h"
#include <GL3\gl3w.h>

#include "Kernel.h"
#include "Scene/Camera.h"
#include "Scene/Node.h"
#include "Scene/Mesh.h"
#include "Scene/Light.h"
#include "Scene/Terrain.h"
#include "Scene/Skybox.h"
#include "File/ShaderLoader.h"
#include "File/MeshLoader.h"
#include "File/TextureLoader.h"

#include "Passes/ScenePass.h"
#include "Passes/GodraysPass.h"
#include "Passes/BloomPass.h"
#include "Passes/FinalPass.h"

/********************************************************
*****												*****
*****		Solar System SCENE :)					*****
*****												*****
*****												*****
*****												*****
*****												*****
*****												*****
********************************************************/

SceneManager::SceneManager()
{
	Scene::Camera::getSingleton(); //Construct camera singleton instance
}

SceneManager::~SceneManager()
{
	Scene::Camera::Shutdown(); //Destruct camera singleton instance
}

void SceneManager::init(const std::string &resource_dir)
{
	this->resource_dir = resource_dir;
	auto kernel = Kernel::getSingleton();
	auto camera = Scene::Camera::getSingleton();

	//Set up the projection matrix, or view_to_clip matrix as it's called here.
	camera->updateProjection(kernel->getWidth(), kernel->getHeight(), 40.0f, 0.1f, 10000.0f);
	camera->setPos(glm::vec3(0.0f, 20.0f, 0.0f));

	//This is the root node in the scenegraph.
	scene = std::make_shared<Scene::Node>();

	//Add skybox around entire scene
	std::vector<std::string> postfix_list;
	postfix_list.push_back("_left.tif");
	postfix_list.push_back("_right.tif");
	postfix_list.push_back("_up.tif");
	postfix_list.push_back("_up.tif");
	postfix_list.push_back("_front.tif");
	postfix_list.push_back("_back.tif");
	auto skybox = std::make_shared<Scene::Skybox>(kernel->getTextureLoader()->loadRelativePath("DawnDusk", postfix_list), 1000.0f);

	//Add a terrain
	auto terrain = std::make_shared<Scene::Terrain>(kernel->getTextureLoader()->loadRelativePath("heightmap.jpg"), "height_map", 0.5f, 0.10f);
	terrain->addTexture(kernel->getTextureLoader()->loadRelativePath("rock.jpg", true), "rock_diffuse_map");
	terrain->addTexture(kernel->getTextureLoader()->loadRelativePath("rock_n.jpg", true), "rock_normal_map");
	terrain->addTexture(kernel->getTextureLoader()->loadRelativePath("sand.jpg", true), "sand_diffuse_map");
	terrain->addTexture(kernel->getTextureLoader()->loadRelativePath("sand_n.jpg", true), "sand_normal_map");
	auto terrain_size = terrain->getTerrainSize();
	terrain->setPosition(glm::vec3(-terrain_size.x/2.0f, 0.0f, -terrain_size.y/2.0f));

	//Add a light to the scene. Notice in the fragment shader that right now we only support 1 light there.
	auto light = std::make_shared<Scene::Light>();
	light->setPosition( glm::vec3(-10,400,-900) );

	//Add a mesh
	auto ship_mesh_test = kernel->getMeshLoader()->load<Scene::Mesh>("Ferox.3DS");
	ship_mesh_test->addTexture(kernel->getTextureLoader()->loadRelativePath("FEROX_DI.tga"), "diffuse_map");
	ship_mesh_test->addTexture(kernel->getTextureLoader()->loadRelativePath("FEROX_BU.tga"), "normal_map");
	ship_mesh_test->setPosition( glm::vec3(0,1.0f,10) );
	ship_mesh_test->setScale(0.01f);
	ship_mesh_test->setPosition( 
		glm::vec3(ship_mesh_test->getPosition().x, 
		terrain->getHeight(ship_mesh_test->getPosition())+ship_mesh_test->getPosition().y, 
		ship_mesh_test->getPosition().z) 
	);

	camera->setPos(
		ship_mesh_test->getPosition() + glm::vec3(0.0f, 1.0f, 10.0f) 
	);

	//Here we add all the nodes to the "root" node called scene. This adds them into the scenegraph right under the root node.
	//For a solar system, this needs to change a bit, but that's an exercise left to you students!
	//For instance, if you want planets as children of sun, do something like this:
	//sun->add(planet);

	scene->add(light);
	scene->add(ship_mesh_test);
	scene->add(terrain);
	scene->add(skybox);

	//////////////////////////////////////////////////
	// SET UP RENDER PASSES
	//////////////////////////////////////////////////
	//1) Scene Pass
	scene_pass = std::make_shared<Pass::ScenePass>(kernel->getWidth(), kernel->getHeight());

	//2) Godrays Pass
	godrays_pass = std::make_shared<Pass::GodraysPass>(kernel->getWidth(), kernel->getHeight());
	
	//3) Bloom Pass
	bloom_pass = std::make_shared<Pass::BloomPass>(kernel->getWidth(), kernel->getHeight());

	//4) Final Pass
	final_pass = std::make_shared<Pass::FinalPass>(kernel->getWidth(), kernel->getHeight());
}

void SceneManager::update(float dt)
{
	auto kernel = Kernel::getSingleton();
	auto camera = Scene::Camera::getSingleton();

	camera->update(
		kernel->getKeyStatus('a'),
		kernel->getKeyStatus('d'),
		kernel->getKeyStatus('s'),
		kernel->getKeyStatus('w'),
		glm::vec2(kernel->getMouseStatus().coords),
		kernel->getMouseStatus().leftPressed,
		dt);

	//Update on scene propagates to all children of scene through the entire scenegraph hierarchy.
	//Override localUpdate function to write any special simulation code for you classes. Like right now,
	//check out the idle spin animation added to the Mesh class by locating it's localUpdate function.
	scene->update(dt);

	//Here we prepare the object_to_world matrices of the entire scenegraph for rendering. This handles
	//calculation of the new object_to_wrold matrix of all nodes internally in the Node class for you.
	//It uses the position and scale vectors, and the orientation quaternion, to create the object_to_world
	//matrix, as well as the parent node's object_to_world matrix. This overrides the previous object_to_world
	//matrix for the node, so whatever it was set to previously is just overwritten with the new version. Because
	//of this, make sure you only change position, scale and orientation to affect the node's object_to_world matrix.
	scene->prepare();
}

void SceneManager::render()
{
	// 1)
	//We start by activating the Scene Pass. Anythin rendered until the end
	//of this pass will be rendered into the Scene Pass FBO.
	scene_pass->begin();
	{
		//Here we render the entire scenegraph. Override the localRender function in your own nodes to add a rendering routine.
		//If you override Mesh with your own special classes, then Mesh will already handle this for you, so you don't have to
		//worry about it.
		scene->render();
	} scene_pass->end(); //Then, when the scene is rendered, we end the Scene Pass

	// 2)
	//Next, godrays occlusion pass. Here we need to render all nodes in the scene 
	//that will occlude lights using the occlusion shader, and all nodes in the scene
	//(skybox) that will cast light on the scene using the occlusion_light shader.
	//This creates an image of black and white colors essentially.
	//(See godrays_pass.fs, and comment in line 57 to see the result of this pass).
	godrays_pass->beginOcclusion();
	{
		scene->renderOcclusion();
	} godrays_pass->endOcclusion();

	// 3)
	//Then the actual godrays...
	//Here we use the occlusion map we created in the previous pass, and draw rays based
	//on light position in screen space (non-homogenous clip-space). See line 123 in GodraysPass.cpp
	//to see how this was calculated. Finally we add the godrays to the FBO texture generated
	//of the scene in the scene pass.
	godrays_pass->begin();
	{
		godrays_pass->render(scene_pass);
	} godrays_pass->end(); //We've now applide godrays to the scene

	// 3)
	//Next, we activate the bloom pass. This renders a fullscreen quad that it
	//applies the scene's FBO texture to, and the performs bloom filtering.
	//Internally this happens over a high-pass color filter to extract lights,
	//then two gaussian blur passes (one for vertical and one for horizontal),
	//and in a final bloom pass, the blurred images are mixed back together with
	//the scene to add the bloom filter effect.
	bloom_pass->begin();
	{
		bloom_pass->render(godrays_pass);
	} bloom_pass->end();  //We've now applied the bloom filter

	// 4)
	//Last, we activate the final pass, that writes to the back buffer
	final_pass->begin();
	{
		//final_pass->render(scene_pass);
		//final_pass->render(godrays_pass);
		final_pass->render(bloom_pass);
	} final_pass->end(); //We've now rendered to the back buffer
}

void SceneManager::reshape(int w, int h)
{
}


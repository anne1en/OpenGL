#include "Kernel.h"
#include "Parser.h"

#include <GL3\gl3w.h>

#include <sstream>

#include "config.h"
#include "SceneManager.h"

#include "File/ShaderLoader.h"
#include "File/MeshLoader.h"
#include "File/TextureLoader.h"

#include "GM/DebugOutput.h"

/********************************************************
*****												*****
*****	This is the file to make changes to	in		*****
*****	order to hook in your own code.				*****
*****	Functions have descriptive comments			*****
*****	to where you should hook in your own		*****
*****	code.										*****
*****												*****
********************************************************/

KernelPtr Kernel::singleton;

KernelPtr Kernel::getSingleton()
{
	if(singleton == nullptr)
		singleton = std::make_shared<Kernel>();
	return singleton;
}

void Kernel::Shutdown()
{
	if(singleton)
		singleton.reset();
}

Kernel::Kernel()
	: running(false)
{
	memset(keystatus, 0, MAX_KEYS);
	mouse.leftPressed = false;
	mouse.rightPressed = false;
}

Kernel::~Kernel()
{
}

std::string Kernel::getGameModeString() const
{
	std::stringstream ss;
	ss << width << "x" << height << ":" << depth << "@" << refresh_rate;
	return ss.str();
}

std::string Kernel::getOpenGLVersionString() const
{
	std::stringstream ss;
	ss << opengl_major_version << "." << opengl_minor_version;
	return ss.str();
}

void Kernel::config(const std::string &resource_dir)
{
	this->resource_dir = resource_dir;

	ini::Parser parser(resource_dir + "ini\\engine.ini");
	width = parser.getInt("Dimensions", "width", ENGINE_DEFAULT_WINDOW_WIDTH);
	height = parser.getInt("Dimensions", "height", ENGINE_DEFAULT_WINDOW_HEIGHT);
	depth = parser.getInt("Dimensions", "depth", ENGINE_DEFAULT_WINDOW_DEPTH);
	refresh_rate = parser.getInt("Dimensions", "refresh_rate", ENGINE_DEFAULT_WINDOW_REFRESH_RATE);
	logic_update_rate = parser.getInt("Dimensions", "logic_update_rate", ENGINE_DEFAULT_LOGIC_UPDATE_RATE);
	fullscreen = parser.getInt("Modes", "fullscreen", ENGINE_DEFAULT_FULLSCREEN);
	game_mode = parser.getInt("Modes", "game", ENGINE_DEFAULT_GAME_MODE);
	opengl_major_version = parser.getInt("OpenGL", "major_version", ENGINE_OPENGL_VERSION_MAJOR);
	opengl_minor_version = parser.getInt("OpenGL", "minor_version", ENGINE_OPENGL_VERSION_MINOR);
}

void Kernel::init(int argc, char** argv)
{
	//////////////////////////////////////////
	// GL3W INITIALIZING
	//////////////////////////////////////////
	GLenum gl3wInitErr = gl3wInit();
	if(gl3wInitErr)
		throw std::runtime_error("Failed to initialize OpenGL!");
	if(gl3wIsSupported(opengl_major_version, opengl_minor_version) == false)
		throw std::runtime_error("Opengl " + getOpenGLVersionString() + " is not supported!");
	wgl3wSwapIntervalEXT(0);

	////////////////////////////////////////
	//ADD CUSTOM INITIALIZATION CODE HERE!
	debug_output = std::make_shared<GM::DebugOutput>();

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	//////////////////////////////////////////
	// FILE SYSTEM INITIALIZING
	//////////////////////////////////////////
	shader_loader = std::make_shared<File::ShaderLoader>(resource_dir+"shaders\\");
	mesh_loader = std::make_shared<File::MeshLoader>(resource_dir+"models\\");
	texture_loader = std::make_shared<File::TextureLoader>(resource_dir+"textures\\");

	//Construct and initialize our scene
	sceneMgr = std::make_shared<SceneManager>();
	sceneMgr->init(resource_dir);
}

void Kernel::run(int start_time, std::function<void()> main_loop_body)
{
	this->start_time = start_time;
	prev_time = start_time;

	running = true;
	while(running)
		main_loop_body();

	Kernel::Shutdown();
}

void Kernel::update(float dt)
{
	/////////////////////////////////////////
	//ADD CUSTOM SIMULATION/LOGIC CODE HERE
	sceneMgr->update(dt);
}

void Kernel::render()
{
	glClearColor(0.9f,0.85f,0.7f,1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	////////////////////////////////////////
	//ADD CUSTOM RENDER CODE HERE
	sceneMgr->render();
}

void Kernel::reshape(int w, int h)
{
#ifdef GM_FORCE_GL3
	glViewport(0, 0, w, h);
#else
	glViewportIndexedf(0, 0.0f, 0.0f, (float)w, (float)h);
#endif
	
	////////////////////////////////////////////////
	//ADD CUSTOM RESHAPE CODE HERE TO CAMERAS, ETC
	sceneMgr->reshape(w,h);
}

void Kernel::inputKeyDown(unsigned char key, int x, int y)
{
	//ESCAPE KEY
	if(key == 27)
		exit();

	keystatus[key] = true;
}

void Kernel::inputKeyUp(unsigned char key, int x, int y)
{
	keystatus[key] = false;
}

void Kernel::input(int key, int x, int y)
{
	// Special keys ALT CTRL etc.
}

void Kernel::motion(int x, int y)
{
	mouse.coords = glm::ivec2(x,y);
}

void Kernel::mousePressed(int button, int state, int x, int y)
{
	const int LEFT_BUTTON = 0;
	const int MIDDLE_BUTTON = 1;
	const int RIGHT_BUTTON = 2;
	const int PRESSED = 0;
	const int RELEASED = 1;
	if(button==LEFT_BUTTON)
	{
		mouse.leftPressed = (state==PRESSED);
	} 
	else if(button==RIGHT_BUTTON)
	{
		mouse.rightPressed = (state==PRESSED);
	}
}

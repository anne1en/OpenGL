#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <functional>

//////////////////////////////////////////////
// Written by Paal Trefall
// Free for all use, but at your own risk!
//////////////////////////////////////////////

namespace File
{
	class ShaderLoader; typedef std::shared_ptr<ShaderLoader> ShaderLoaderPtr;
	class MeshLoader; typedef std::shared_ptr<MeshLoader> MeshLoaderPtr;
	class TextureLoader; typedef std::shared_ptr<TextureLoader> TextureLoaderPtr;
}
namespace GM
{
	class DebugOutput; typedef std::shared_ptr<DebugOutput> DebugOutputPtr;
}

class SceneManager; typedef std::shared_ptr<SceneManager> SceneManagerPtr;
class Kernel; typedef std::shared_ptr<Kernel> KernelPtr;
class Kernel
{
public:
	static KernelPtr getSingleton();
	~Kernel();

	void config(const std::string &resource_dir);
	void init(int argc, char** argv);

	void run(int start_time, std::function<void()> main_loop_body);
	void exit() { running = false; }
	bool isRunning() const { return running; }

	void update(float dt);
	void render();
	void reshape(int w, int h);
	void inputKeyDown(unsigned char key, int x, int y);
	void inputKeyUp(unsigned char key, int x, int y);
	void input(int key, int x, int y);
	void motion(int x, int y);
	void mousePressed(int button, int state, int x, int y);

	int getPrevTime(int cur_time) { auto tmp_time = prev_time; prev_time = cur_time; return tmp_time; }
	int getStartTime() const { return start_time; }
	int getElapsedTime(int cur_time) const { return cur_time - start_time; }
	int getTimeSincePrevFrame(int cur_time) { return cur_time - getPrevTime(cur_time); }

public:
	const std::string &getResourceDir() const { return resource_dir; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	int getDepth() const { return depth; }
	int getRefreshRate() const { return refresh_rate; }
	int getLogicUpdateRate() const { return logic_update_rate; }
	int getFullscreen() const { return fullscreen; }
	int getGameMode() const { return game_mode; }
	std::string getGameModeString() const;
	int getOpenGLVersionMajor() const { return opengl_major_version; }
	int getOpenGLVersionMinor() const { return opengl_minor_version; }
	std::string getOpenGLVersionString() const;

public:
	File::ShaderLoaderPtr getShaderLoader() const { return shader_loader; }
	File::MeshLoaderPtr getMeshLoader() const { return mesh_loader; }
	File::TextureLoaderPtr getTextureLoader() const { return texture_loader; }

	bool getKeyStatus(char key) { if(key < MAX_KEYS) return keystatus[key]; else return false; }

	struct MouseDef
	{
		bool leftPressed;
		bool rightPressed;
		glm::ivec2 coords;
	};
	const MouseDef &getMouseStatus() { return mouse; }

private:
#if(_MSC_VER >= 1700)
	friend class std::_Ref_count_obj<Kernel>;
#else
	friend class std::tr1::_Ref_count_obj<Kernel>;
#endif
	static void Shutdown();
	static KernelPtr singleton;
	Kernel();

	std::string resource_dir;

	int width;
	int height;
	int depth;
	int refresh_rate;
	int logic_update_rate;
	int fullscreen;
	int game_mode;
	int opengl_major_version;
	int opengl_minor_version;

	bool running;
	int start_time;
	int prev_time;

private:
	File::ShaderLoaderPtr shader_loader;
	File::MeshLoaderPtr mesh_loader;
	File::TextureLoaderPtr texture_loader;

	const static int MAX_KEYS = 256;
	bool keystatus[MAX_KEYS];
	MouseDef mouse;

private:
	GM::DebugOutputPtr debug_output;

	SceneManagerPtr sceneMgr;
};

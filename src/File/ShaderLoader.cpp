#include "ShaderLoader.h"

#include <sstream>
#include <fstream>

using namespace File;

ShaderLoader::ShaderLoader(const std::string &base_dir)
	: base_dir(base_dir)
{
}

GM::ShaderPtr ShaderLoader::load(const std::string &vs_filename, const std::string &gs_filename, const std::string &fs_filename)
{
	auto vs_contents = loadContents(vs_filename);
	auto gs_contents = loadContents(gs_filename);
	auto fs_contents = loadContents(fs_filename);
	return std::make_shared<GM::Shader>(vs_contents, gs_contents, fs_contents);
}

std::string ShaderLoader::loadContents(const std::string &filename)
{
	if(filename.empty())
		return std::string();

	std::ifstream stream;
	stream.open(base_dir+filename);
	if(stream.bad())
	{
		stream.close();
		throw std::runtime_error("Failed to load file " + filename);
	}

	std::stringstream buffer;
	buffer << stream.rdbuf();
	stream.close();

	return buffer.str();
}

void ShaderLoader::push_bind(const GM::ShaderPtr &shader)
{
	boundShaderStack.push(shader);
	shader->bind();
}

void ShaderLoader::pop_bind()
{
	auto old_shader = boundShaderStack.top();
	boundShaderStack.pop();
	if(boundShaderStack.empty())
	{
		old_shader->unbind();
		return;
	}

	auto shader = boundShaderStack.top();
	shader->bind();
}

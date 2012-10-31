#include "Uniform.h"
#include <GL3\gl3w.h>
#include <iostream>
#include <glm/ext.hpp>

using namespace GM;

Uniform::Uniform(unsigned int program, const std::string &name) 
	: program(program), location(-1), name(name)
{
	location = glGetUniformLocation(program, name.c_str());
	//if(location < 0)
	//	throw std::runtime_error("Couldn't find uniform with name " + name + " in shader program!");
}

void Uniform::bind(int data)
{
#ifdef GM_FORCE_GL3
	glUniform1i(location, data);
#else
	glProgramUniform1i(program, location, data);
#endif
}

void Uniform::bind(float data)
{
#ifdef GM_FORCE_GL3
	glUniform1f(location, data);
#else
	glProgramUniform1f(program, location, data);
#endif
}

void Uniform::bind(const glm::vec2 &data)
{
#ifdef GM_FORCE_GL3
	glUniform2fv(location, 1, glm::value_ptr(data));
#else
	glProgramUniform2fv(program, location, 1, glm::value_ptr(data));
#endif
}

void Uniform::bind(const glm::vec3 &data)
{
#ifdef GM_FORCE_GL3
	glUniform3fv(location, 1, glm::value_ptr(data));
#else
	glProgramUniform3fv(program, location, 1, glm::value_ptr(data));
#endif
}

void Uniform::bind(const glm::mat3 &data)
{
#ifdef GM_FORCE_GL3
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(data));
#else
	glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, glm::value_ptr(data));
#endif
}

void Uniform::bind(const glm::mat4 &data)
{
#ifdef GM_FORCE_GL3
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(data));
#else
	glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, glm::value_ptr(data));
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Uniform::Uniform(const std::string &name) 
	: program(0), location(-1), name(name)
{
}

void Uniform::bind(int data, unsigned int program)
{
	if(location < 0)
		location = glGetUniformLocation(program, name.c_str());

	this->program = program;
	bind(data);
}

void Uniform::bind(float data, unsigned int program)
{
	if(location < 0)
		location = glGetUniformLocation(program, name.c_str());

	this->program = program;
	bind(data);
}

void Uniform::bind(const glm::vec2 &data, unsigned int program)
{
	if(location < 0)
		location = glGetUniformLocation(program, name.c_str());

	this->program = program;
	bind(data);
}

void Uniform::bind(const glm::vec3 &data, unsigned int program)
{
	if(location < 0)
		location = glGetUniformLocation(program, name.c_str());

	this->program = program;
	bind(data);
}

void Uniform::bind(const glm::mat3 &data, unsigned int program)
{
	if(location < 0)
		location = glGetUniformLocation(program, name.c_str());

	this->program = program;
	bind(data);
}

void Uniform::bind(const glm::mat4 &data, unsigned int program)
{
	if(location < 0)
		location = glGetUniformLocation(program, name.c_str());

	this->program = program;
	bind(data);
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
int Uniform::findLocation(unsigned int program)
{
	return glGetUniformLocation(program, name.c_str());
}

void Uniform::bind(int data, unsigned int program, int location)
{
	this->program = program;
	this->location = location;
	bind(data);
}

void Uniform::bind(float data, unsigned int program, int location)
{
	this->program = program;
	this->location = location;
	bind(data);
}

void Uniform::bind(const glm::vec2 &data, unsigned int program, int location)
{
	this->program = program;
	this->location = location;
	bind(data);
}

void Uniform::bind(const glm::vec3 &data, unsigned int program, int location)
{
	this->program = program;
	this->location = location;
	bind(data);
}

void Uniform::bind(const glm::mat3 &data, unsigned int program, int location)
{
	this->program = program;
	this->location = location;
	bind(data);
}

void Uniform::bind(const glm::mat4 &data, unsigned int program, int location)
{
	this->program = program;
	this->location = location;
	bind(data);
}

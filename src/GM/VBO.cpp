#include "VBO.h"

using namespace GM;

VBO::VBO(const unsigned int &size, const unsigned int &draw_type)
	: size(size), offset(0)
{
	glGenBuffers(1, &handle);
	bind();
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, draw_type);
}

VBO::~VBO()
{
	glDeleteBuffers(1, &handle);
}

void VBO::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, handle);
	offset = 0;
}

void VBO::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

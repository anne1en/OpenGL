#include "TexCUBE.h"

using namespace GM;

TexCUBE::TexCUBE(const TexCUBEParams &tex_params)
	: faces_set(6)
{
#ifdef GL_TEXTURE_CUBE_MAP_SEAMLESS
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif
	glGenTextures(1, &handle);
	bind();
	for(unsigned int i = 0; i < tex_params.params.size(); i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 
			0, tex_params.params[i].internal_format, tex_params.params[i].w, tex_params.params[i].h, 
			0, tex_params.params[i].format, tex_params.params[i].type, tex_params.params[i].data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

TexCUBE::TexCUBE()
	: faces_set(0)
{
#ifdef GL_TEXTURE_CUBE_MAP_SEAMLESS
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif
	glGenTextures(1, &handle);
	bind();
}

TexCUBE::~TexCUBE()
{
	glDeleteTextures(1, &handle);
}

void TexCUBE::setFace(unsigned int side, const T2DTexParams &param)
{
	glTexImage2D(side, 
			0, param.internal_format, param.w, param.h, 
			0, param.format, param.type, param.data);
	faces_set++;
}

void TexCUBE::finished()
{
	if(faces_set != 6)
		return;

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void TexCUBE::bind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, handle);
}

void TexCUBE::unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

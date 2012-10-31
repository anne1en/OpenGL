#include "TextureLoader.h"

#include <IL\il.h>
#include <IL\ilu.h>

#include <algorithm>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>

using namespace File;

TextureLoader::TextureLoader(const std::string &base_dir)
	: base_dir(base_dir)
{
	ilInit();
}

GM::Tex2DPtr TextureLoader::loadAbsolutePath(const std::string &filename, bool isRepeated)
{
	return load(filename, isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE );
}
GM::Tex2DPtr TextureLoader::loadRelativePath(const std::string &filename, bool isRepeated)
{
	return load(base_dir+filename, isRepeated ? GL_REPEAT : GL_CLAMP_TO_EDGE );
}

GM::Tex2DPtr TextureLoader::load(const std::string &filename, unsigned int wrap_mode)
{
	unsigned int il_handle = 0;
	internal_tex_data data = internal_load(il_handle, filename);

	GM::T2DTexParams params(data.format, data.format, GL_UNSIGNED_BYTE, data.bpp, data.w, data.h, wrap_mode, data.data);
	auto tex = std::make_shared<GM::Tex2D>(params);
	//ilDeleteImages(1, &il_handle);
	return tex;
}

GM::TexCUBEPtr TextureLoader::loadAbsolutePath(const std::string &filename, const std::vector<std::string> &postfix_list)
{
	return load(filename, postfix_list);
}

GM::TexCUBEPtr TextureLoader::loadRelativePath(const std::string &filename, const std::vector<std::string> &postfix_list)
{
	return load(base_dir+filename, postfix_list);
}

GM::TexCUBEPtr TextureLoader::load(const std::string &filename, const std::vector<std::string> &postfix_list)
{
	//unsigned int il_handle = 0;
	std::vector<unsigned int> il_handles;
	//GM::TexCUBEParams cube_params;
	auto tex = std::make_shared<GM::TexCUBE>();
	for(unsigned int i = 0; i < postfix_list.size(); i++)
	{
		unsigned int il_handle = 0;
		internal_tex_data data = internal_load(il_handle, filename+postfix_list[i]);
		//il_handles.push_back(il_handle);
		GM::T2DTexParams params(data.format, data.format, GL_UNSIGNED_BYTE, data.bpp, data.w, data.h, GL_CLAMP_TO_EDGE, data.data);
		if(i==0) tex->setFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X, params);
		if(i==1) tex->setFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, params);
		if(i==2) tex->setFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, params);
		if(i==3) tex->setFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, params);
		if(i==4) tex->setFace(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, params);
		if(i==5) tex->setFace(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, params);
		//ilDeleteImages(1, &il_handle);
		//cube_params.params.push_back(params);
	}
	tex->finished();
	//auto tex = std::make_shared<GM::TexCUBE>(cube_params);
	//ilDeleteImages(il_handles.size(), &il_handles[0]);
	return tex;
}

TextureLoader::internal_tex_data TextureLoader::internal_load(unsigned int &il_handle, const std::string &filename)
{
    ilGenImages(1, &il_handle);

    ilBindImage(il_handle);
    int success = ilLoadImage(filename.c_str());
    if(!success)
    {
            ILenum err = ilGetError();
            return internal_tex_data();
    }

    if(ilGetInteger(IL_IMAGE_ORIGIN) != IL_ORIGIN_LOWER_LEFT)
    {     
            iluFlipImage();
    }

    success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    if(!success)
    {
            ILenum err = ilGetError();
            return internal_tex_data();
    }

	internal_tex_data data;
    data.bpp = ilGetInteger(IL_IMAGE_BPP);
    data.w = ilGetInteger(IL_IMAGE_WIDTH);
    data.h = ilGetInteger(IL_IMAGE_HEIGHT);
    data.format = ilGetInteger(IL_IMAGE_FORMAT);
	data.type = ilGetInteger(IL_IMAGE_TYPE);
    data.data = ilGetData();

	return data;
}

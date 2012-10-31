#pragma once

#include "../GM/Tex2D.h"
#include "../GM/TexCUBE.h"

#include <memory>
#include <string>

namespace File
{
	class TextureLoader;
	typedef std::shared_ptr<TextureLoader> TextureLoaderPtr;

	class TextureLoader
	{
	public:
		TextureLoader(const std::string &base_dir);

		GM::Tex2DPtr loadAbsolutePath(const std::string &filename, bool isRepeated = false);
		GM::Tex2DPtr loadRelativePath(const std::string &filename, bool isRepeated = false);

		GM::TexCUBEPtr loadAbsolutePath(const std::string &filename, const std::vector<std::string> &postfix_list);
		GM::TexCUBEPtr loadRelativePath(const std::string &filename, const std::vector<std::string> &postfix_list);

	private:
		GM::Tex2DPtr load(const std::string &filename, unsigned int wrap_mode = GL_CLAMP_TO_EDGE);

		GM::TexCUBEPtr load(const std::string &filename, const std::vector<std::string> &postfix_list);

		struct internal_tex_data
		{
			int bpp;
			int w;
			int h;
			int format;
			int type;
			unsigned char *data;
			internal_tex_data() : bpp(0), w(0), h(0), format(0), data(nullptr) {}
		};
		internal_tex_data internal_load(unsigned int &il_handle, const std::string &filename);
		std::string base_dir;
	};
}

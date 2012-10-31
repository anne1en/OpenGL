#pragma once

#include "Tex2D.h"
#include <GL3/gl3w.h>

#include <memory>
#include <vector>
#include <string>

namespace GM
{
	class TexCUBE;
	typedef std::shared_ptr<TexCUBE> TexCUBEPtr;
	
	struct TexCUBEParams
	{
		std::vector<T2DTexParams> params;
	};

	class TexCUBE
	{
	public:
		TexCUBE(const TexCUBEParams &tex_params);
		TexCUBE();
		~TexCUBE();

		void setFace(unsigned int, const T2DTexParams &param);
		void finished();

		void bind();
		void unbind();

		unsigned int getHandle() const { return handle; }

	private:
		unsigned int handle;
		unsigned int faces_set;
	};
}
